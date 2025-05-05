#include "SearchServer.h"
#include "InvertedIndex.h"
#include "ConverterJSON.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

class SearchServerTest : public ::testing::Test {
protected:
    void SetUp() override {
        createTestFiles();
        createTestConfig();
        _index.UpdateDocumentBase();
    }

    void TearDown() override {
        for (const auto& file : test_files) fs::remove(file);
        if (fs::exists("config.json")) fs::remove("config.json");
        if (fs::exists("answers.json")) fs::remove("answers.json");
    }

    void createTestFiles() {
        test_files = {"file1.txt", "file2.txt", "file3.txt"};

        std::ofstream("file1.txt") << "apple banana apple";
        std::ofstream("file2.txt") << "banana cherry";
        std::ofstream("file3.txt") << "cherry cherry cherry";
    }

    void createTestConfig() {
        std::ofstream("config.json") << R"({
            "config": {"name":"Test","version":"1.0","max_responses":5},
            "files": ["file1.txt", "file2.txt", "file3.txt"]
        })";
    }

    ConverterJSON converter;
    InvertedIndex _index;
    SearchServer server{_index};
    std::vector<std::string> test_files;
};

TEST_F(SearchServerTest, SearchReturnsCorrectResultsForSingleWord) {
    auto results = server.search({"apple"});

    ASSERT_EQ(results.size(), 1);
    ASSERT_EQ(results[0].size(), 1);
    EXPECT_EQ(results[0][0].doc_id, 0); // apple contains only in file1.txt
    EXPECT_FLOAT_EQ(results[0][0].rank, 1.0f); // 100%
}

TEST_F(SearchServerTest, SearchReturnsCorrectResultsForMultiWordQuery) {
    auto results = server.search({"banana cherry"});

    ASSERT_EQ(results.size(), 1);
    ASSERT_EQ(results[0].size(), 3);

    EXPECT_EQ(results[0][0].doc_id, 2); // file3.txt (cherry x3) - max rank
    EXPECT_EQ(results[0][1].doc_id, 1); // file2.txt (banana+cherry)
    EXPECT_EQ(results[0][2].doc_id, 0); // file1.txt (banana x1)

    // max rank: file3.txt (cherry x3 = 3)
    EXPECT_FLOAT_EQ(results[0][0].rank, 1.0f);      // 3/3 = 1.0
    EXPECT_FLOAT_EQ(results[0][1].rank, 2.0f/3);    // (1+1)/3 ≈ 0.666...
    EXPECT_FLOAT_EQ(results[0][2].rank, 1.0f/3);    // 1/3 ≈ 0.333...
}

TEST_F(SearchServerTest, SearchHandlesPunctuationInQueries) {
    auto results = server.search({"apple,", "banana! ?"});

    ASSERT_EQ(results.size(), 2);

    // request "apple,"
    ASSERT_EQ(results[0].size(), 1);
    EXPECT_EQ(results[0][0].doc_id, 0);

    // request "banana! ?"
    ASSERT_EQ(results[1].size(), 2); // banana in file1, file2
}

TEST_F(SearchServerTest, SearchReturnsEmptyForUnknownWords) {
    auto results = server.search({"nonexistent"});
    ASSERT_EQ(results.size(), 1);
    EXPECT_TRUE(results[0].empty());
}

TEST_F(SearchServerTest, SearchHandlesEmptyQueries) {
    auto results = server.search({"", "   ", ",.!"});
    ASSERT_EQ(results.size(), 3);
    for (const auto& result : results) {
        EXPECT_TRUE(result.empty());
    }
}

TEST_F(SearchServerTest, SearchProcessesMultipleRequests) {
    auto results = server.search({"apple", "banana", "cherry"});

    ASSERT_EQ(results.size(), 3);

    EXPECT_EQ(results[0][0].doc_id, 0); // apple
    EXPECT_EQ(results[1].size(), 2);    // banana
    EXPECT_EQ(results[2].size(), 2);    // cherry
}
