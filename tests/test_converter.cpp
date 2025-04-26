#include <gtest/gtest.h>
#include "ConverterJSON.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

class ConverterJSONTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::ofstream("requests.json") << R"({"requests": ["query1", "query2"]})";
    }

    void TearDown() override {
        // Удаляем временные файлы
        fs::remove("requests.json");
        fs::remove("answers.json");
    }
};

TEST_F(ConverterJSONTest, ReadsRequestsCorrectly) {
    ConverterJSON converter;
    auto requests = converter.GetRequests();

    ASSERT_EQ(requests.size(), 2);
    EXPECT_EQ(requests[0], "query1");
    EXPECT_EQ(requests[1], "query2");
}

TEST_F(ConverterJSONTest, WritesAnswersCorrectly) {
    ConverterJSON converter;
    std::vector<std::vector<std::pair<int, float>>> answers = {
        {{1, 0.9f}, {2, 0.5f}},
        {{3, 0.2f}}
    };

    converter.putAnswers(answers);

    std::ifstream file("answers.json");
    ASSERT_TRUE(file.is_open());

    nlohmann::json data;
    file >> data;

    EXPECT_EQ(data.size(), 2);
    EXPECT_EQ(data[0].size(), 2);
    EXPECT_EQ(data[1].size(), 1);
}
