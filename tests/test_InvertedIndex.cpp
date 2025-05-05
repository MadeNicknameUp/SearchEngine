#include "InvertedIndex.h"
#include "ConverterJSON.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

class InvertedIndexTest : public ::testing::Test {
protected:
    void SetUp() override {
        createTestFiles();
        createTestConfig();
    }

    void TearDown() override {
        for (const auto& file : test_files) {
            if (fs::exists(file)) fs::remove(file);
        }
        if (fs::exists("config.json")) fs::remove("config.json");
        if (fs::exists("answers.json")) fs::remove("answers.json");
    }

    void createTestFiles() {
        test_files = {
            "test_file1.txt",
            "test_file2.txt",
            "test_file3.txt"
        };

        std::ofstream file1(test_files[0]);
        file1 << "hello world hello";
        file1.close();

        std::ofstream file2(test_files[1]);
        file2 << "world of warcraft";
        file2.close();

        std::ofstream file3(test_files[2]);
        file3 << "test test test";
        file3.close();
    }

    void createTestConfig() {
        std::ofstream config("config.json");
        config << R"({
            "config": {
                "name": "TestSearchEngine",
                "version": "1.0",
                "max_responses": 5
            },
            "files": [
                "test_file1.txt",
                "test_file2.txt",
                "test_file3.txt"
            ]
        })";
        config.close();
    }

    std::vector<std::string> test_files;
};

TEST_F(InvertedIndexTest, UpdateDocumentBaseProcessesAllFiles) {
    ConverterJSON converter;
    InvertedIndex index;

    index.UpdateDocumentBase();

    EXPECT_FALSE(index.GetWordCount("hello").empty());
    EXPECT_FALSE(index.GetWordCount("world").empty());
    EXPECT_FALSE(index.GetWordCount("of").empty());
    EXPECT_FALSE(index.GetWordCount("warcraft").empty());
    EXPECT_FALSE(index.GetWordCount("test").empty());
}

TEST_F(InvertedIndexTest, GetWordCountReturnsCorrectCounts) {
    ConverterJSON converter;
    InvertedIndex index;

    index.UpdateDocumentBase();

    const auto& hello_counts = index.GetWordCount("hello");
    EXPECT_EQ(hello_counts.size(), 1);
    EXPECT_EQ(hello_counts.at(0), 2);

    const auto& world_counts = index.GetWordCount("world");
    EXPECT_EQ(world_counts.size(), 2);
    EXPECT_EQ(world_counts.at(0), 1);
    EXPECT_EQ(world_counts.at(1), 1);

    const auto& test_counts = index.GetWordCount("test");
    EXPECT_EQ(test_counts.size(), 1);
    EXPECT_EQ(test_counts.at(2), 3);
}

TEST_F(InvertedIndexTest, GetWordCountReturnsEmptyForUnknownWord) {
    ConverterJSON converter;
    InvertedIndex index;

    index.UpdateDocumentBase();

    const auto& unknown_counts = index.GetWordCount("unknownword");
    EXPECT_TRUE(unknown_counts.empty());
}

TEST_F(InvertedIndexTest, ProcessFileHandlesEmptyFile) {
    std::string empty_file = "empty_file.txt";
    {
        std::ofstream file(empty_file);
    }

    std::ofstream config("config.json", std::ios::trunc);
    config << R"({
        "config": {
            "name": "TestSearchEngine",
            "version": "1.0",
            "max_responses": 5
        },
        "files": [
            "test_file1.txt",
            "empty_file.txt"
        ]
    })";
    config.close();

    ConverterJSON converter;
    InvertedIndex index;

    index.UpdateDocumentBase();

    const auto& counts = index.GetWordCount("anyword");
    EXPECT_TRUE(counts.empty());
}

TEST_F(InvertedIndexTest, ThreadSafety) {
    std::vector<std::string> thread_test_files;
    for (int i = 0; i < 5; ++i) {
        std::string filename = "thread_test_file_" + std::to_string(i) + ".txt";
        std::ofstream file(filename);
        file << "thread thread thread";
        file.close();
        thread_test_files.push_back(filename);
    }

    std::ofstream config("config.json", std::ios::trunc);
    config << R"({"config":{"name":"Test","version":"1.0","max_responses":5},"files":[)";
    for (size_t i = 0; i < thread_test_files.size(); ++i) {
        if (i != 0) config << ",";
        config << "\"" << thread_test_files[i] << "\"";
    }
    config << "]}";
    config.close();

    ConverterJSON converter;
    InvertedIndex index;

    index.UpdateDocumentBase();

    const auto& counts = index.GetWordCount("thread");
    EXPECT_EQ(counts.size(), thread_test_files.size());
    for (size_t i = 0; i < thread_test_files.size(); ++i) {
        EXPECT_EQ(counts.at(i), 3);
    }

    for (const auto& file : thread_test_files) {
        fs::remove(file);
    }
}
