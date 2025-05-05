#include "InvertedIndex.h"
#include "SearchServer.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

class SearchServerExtendedTest : public ::testing::Test {
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
            "test_file3.txt",
            "test_file4.txt"
        };

        std::ofstream("test_file1.txt") << "milk milk milk milk water water water";
        std::ofstream("test_file2.txt") << "milk water water";
        std::ofstream("test_file3.txt") << "milk milk milk milk milk water water water water water";
        std::ofstream("test_file4.txt") << "americano cappuccino";
    }

    void createTestConfig() {
        std::ofstream("config.json") << R"({
            "config": {
                "name": "TestSearchEngine",
                "version": "1.0",
                "max_responses": 5
            },
            "files": [
                "test_file1.txt",
                "test_file2.txt",
                "test_file3.txt",
                "test_file4.txt"
            ]
        })";
    }

    std::vector<std::string> test_files;
};

TEST_F(SearchServerExtendedTest, TestSimple) {
    InvertedIndex idx;
    idx.UpdateDocumentBase();

    SearchServer srv(idx);
    std::vector<std::string> request = {"milk water", "sugar"};

    std::vector<std::vector<RelativeIndex>> result = srv.search(request);

    ASSERT_EQ(result.size(), 2);

    ASSERT_EQ(result[0].size(), 3);
    EXPECT_EQ(result[0][0].doc_id, 2);
    EXPECT_NEAR(result[0][0].rank, 1.0f, 0.001f);
    EXPECT_EQ(result[0][1].doc_id, 0);
    EXPECT_NEAR(result[0][1].rank, 0.7f, 0.001f);
    EXPECT_EQ(result[0][2].doc_id, 1);
    EXPECT_NEAR(result[0][2].rank, 0.3f, 0.001f);

    EXPECT_TRUE(result[1].empty());
}

TEST_F(SearchServerExtendedTest, TestBasicRanking) {
    std::vector<std::string> test_docs = {
        "moscow is the capital of russia",
        "welcome to moscow the capital of russia",
        "paris is the capital of france"
    };

    for (size_t i = 0; i < test_docs.size(); ++i) {
        std::string filename = "testdoc_" + std::to_string(i) + ".txt";
        std::ofstream(filename) << test_docs[i];
        test_files.push_back(filename);
    }

    std::ofstream config("config.json", std::ios::trunc);
    config << R"({"config":{"name":"Test","version":"1.0","max_responses":5},"files":[)";
    for (size_t i = 0; i < test_files.size(); ++i) {
        if (i != 0) config << ",";
        config << "\"" << test_files[i] << "\"";
    }
    config << "]}";
    config.close();

    InvertedIndex idx;
    idx.UpdateDocumentBase();

    SearchServer srv(idx);
    std::vector<std::string> request = {"moscow russia"};

    std::vector<std::vector<RelativeIndex>> result = srv.search(request);

    ASSERT_EQ(result.size(), 1);
    ASSERT_GT(result[0].size(), 0);

    for (size_t i = 1; i < result[0].size(); ++i) {
        EXPECT_LE(result[0][i].rank, result[0][i-1].rank);
    }

    if (!result[0].empty()) {
        size_t top_doc_id = result[0][0].doc_id;
        std::ifstream file(test_files[top_doc_id]);
        std::string content((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());

        EXPECT_TRUE(content.find("moscow") != std::string::npos ||
                   content.find("russia") != std::string::npos);
    }
}
