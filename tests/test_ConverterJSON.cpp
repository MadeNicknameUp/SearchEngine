#include "ConverterJSON.h"
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

class ConverterJSONTest : public ::testing::Test {
protected:
    void SetUp() override {
        createTestConfig();
        createTestRequests();
    }

    void TearDown() override {
        if (fs::exists("config.json")) fs::remove("config.json");
        if (fs::exists("requests.json")) fs::remove("requests.json");
        if (fs::exists("answers.json")) fs::remove("answers.json");
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
                "../resources/file1.txt",
                "../resources/file2.txt",
                "../resources/file3.txt",
                "../resources/file4.txt",
                "../resources/file5.txt"
            ]
        })";
        config.close();
    }

    void createTestRequests() {
        std::ofstream requests("requests.json");
        requests << R"({
            "requests": [
                "some word",
                "some",
                "word",
                "nonsense"
            ]
        })";
        requests.close();
    }

    void createEmptyConfig() {
        std::ofstream config("config.json");
        config << "{}";
        config.close();
    }

    void createInvalidConfig() {
        std::ofstream config("config.json");
        config << "{ invalid json }";
        config.close();
    }

    ConverterJSON converter;
};

TEST_F(ConverterJSONTest, GetTextDocumentsReturnsCorrectFiles) {
    auto documents = converter.GetTextDocuments();
    ASSERT_EQ(documents.size(), 5);  // Изменили ожидание с 3 на 5
    EXPECT_EQ(documents[0], "../resources/file1.txt");
    EXPECT_EQ(documents[1], "../resources/file2.txt");
    EXPECT_EQ(documents[2], "../resources/file3.txt");
    EXPECT_EQ(documents[3], "../resources/file4.txt");
    EXPECT_EQ(documents[4], "../resources/file5.txt");
}

TEST_F(ConverterJSONTest, GetRequestsReturnsCorrectRequests) {
    auto requests = converter.GetRequests();
    ASSERT_EQ(requests.size(), 4);
    EXPECT_EQ(requests[0], "some word");
    EXPECT_EQ(requests[1], "some");
    EXPECT_EQ(requests[2], "word");
    EXPECT_EQ(requests[3], "nonsense");
}

TEST_F(ConverterJSONTest, GetResponsesLimitReturnsCorrectValue) {
    EXPECT_EQ(converter.GetResponsesLimit(), 5);
}

TEST_F(ConverterJSONTest, GetRequestsReturnsEmptyVectorWhenNoRequestsFile) {
    fs::remove("requests.json");
    auto requests = converter.GetRequests();
    EXPECT_TRUE(requests.empty());
}

TEST_F(ConverterJSONTest, GetRequestsReturnsEmptyVectorWhenNoRequestsSection) {
    std::ofstream requests("requests.json");
    requests << "{}";
    requests.close();

    auto requestsList = converter.GetRequests();
    EXPECT_TRUE(requestsList.empty());
}

TEST_F(ConverterJSONTest, PutAnswersCreatesValidJsonFile) {
    std::vector<std::vector<std::pair<int, float>>> answers = {
        {{1, 0.9f}, {2, 0.8f}},
        {{3, 0.7f}},
        {}
    };

    converter.putAnswers(answers);

    ASSERT_TRUE(fs::exists("answers.json"));

    std::ifstream file("answers.json");
    nlohmann::json result;
    file >> result;

    EXPECT_TRUE(result.contains("answers"));
    EXPECT_EQ(result["answers"].size(), 3);
    EXPECT_EQ(result["answers"]["request1"]["result"], "true");
    EXPECT_EQ(result["answers"]["request2"]["result"], "true");
    EXPECT_EQ(result["answers"]["request3"]["result"], "false");
}

TEST_F(ConverterJSONTest, ThrowsWhenConfigFileMissing) {
    fs::remove("config.json");
    EXPECT_THROW(converter.GetTextDocuments(), std::runtime_error);
    EXPECT_THROW(converter.GetResponsesLimit(), std::runtime_error);
}

TEST_F(ConverterJSONTest, ThrowsWhenConfigFileEmpty) {
    createEmptyConfig();
    EXPECT_THROW(converter.GetTextDocuments(), std::runtime_error);
    EXPECT_THROW(converter.GetResponsesLimit(), std::runtime_error);
}

TEST_F(ConverterJSONTest, ThrowsWhenConfigFileInvalid) {
    createInvalidConfig();
    EXPECT_THROW(converter.GetTextDocuments(), std::runtime_error);
    EXPECT_THROW(converter.GetResponsesLimit(), std::runtime_error);
}

TEST_F(ConverterJSONTest, ThrowsWhenConfigMissingRequiredSections) {
    std::ofstream config("config.json");
    config << R"({"config": {"name": "Test"}})";
    config.close();

    EXPECT_THROW(converter.GetTextDocuments(), std::runtime_error);
    EXPECT_THROW(converter.GetResponsesLimit(), std::runtime_error);
}

TEST_F(ConverterJSONTest, PutAnswersHandlesEmptyInput) {
    converter.putAnswers({});
    ASSERT_TRUE(fs::exists("answers.json"));

    std::ifstream file("answers.json");
    nlohmann::json result;
    file >> result;

    EXPECT_TRUE(result.contains("answers"));
    EXPECT_TRUE(result["answers"].empty());
}
