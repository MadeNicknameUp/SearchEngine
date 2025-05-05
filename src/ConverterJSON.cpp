#include "ConverterJSON.h"
#include <filesystem>
#include <iostream>
#include <fstream>

using std::runtime_error;
using std::exception;
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;
using std::pair;
using std::ios;

namespace fs = std::filesystem;

void ConverterJSON::loadConfig() const {
    if (config_loaded) return;

    if (!fs::exists("config.json")) {
        throw runtime_error("config file is missing");
    }

    config_cache = safeParse("config.json");

    if (config_cache.empty()) {
        throw runtime_error("config file is empty");
    }

    if (!config_cache.contains("config")) {
        throw runtime_error("config file: missing 'config' section");
    }

    const auto& config = config_cache["config"];
    if (!config.contains("name") || !config.contains("version") || !config.contains("max_responses")) {
        throw runtime_error("config file: missing required fields in 'config' section");
    }

    if (!config_cache.contains("files")) {
        throw runtime_error("config file: missing 'files' section");
    }

    if (!config_cache["files"].is_array() || config_cache["files"].empty()) {
        throw runtime_error("config file: 'files' must be non-empty array");
    }

    config_loaded = true;
}

void ConverterJSON::loadRequests() const {
    if (requests_loaded) return;

    if (!fs::exists("requests.json")) {
        requests_cache = json::object();
    } else {
        requests_cache = safeParse("requests.json");
    }
    requests_loaded = true;
}

nlohmann::json ConverterJSON::safeParse(const string& filename) const {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open " + filename);
    }

    try {
        json data;
        file >> data;
        return data;
    } catch (const exception& e) {
        throw runtime_error("Invalid JSON in " + filename + ": " + e.what());
    }
}

vector<string> ConverterJSON::GetTextDocuments() const {
    loadConfig();
    return config_cache["files"].get<vector<string>>();
}

vector<string> ConverterJSON::GetRequests() const {
    loadRequests();

    if (!requests_cache.contains("requests")) {
        return {};
    }

    return requests_cache["requests"].get<vector<string>>();
}

void ConverterJSON::putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers) const {
    std::ofstream file("answers.json", std::ios::trunc);

    if (!file) {
        std::cerr << "\033[1;31mError: Can't open answers.json\033[0m" << std::endl;
        return;
    }

    try {
        nlohmann::json result;
        result["answers"] = nlohmann::json::object();

        for (size_t i = 0; i < answers.size(); ++i) {
            std::string request_id = "request" + std::to_string(i + 1);

            if (answers[i].empty()) {
                result["answers"][request_id] = {{"result", "false"}};
            } else {
                nlohmann::json relevance;
                for (const auto& [doc_id, rank] : answers[i]) {
                    relevance.push_back({
                        {"docid", doc_id},
                        {"rank", rank}
                    });
                }

                if (answers[i].size() > 1) {
                    result["answers"][request_id] = {
                        {"result", "true"},
                        {"relevance", relevance}
                    };
                } else {
                    result["answers"][request_id] = {
                        {"result", "true"},
                        {"docid", answers[i][0].first},
                        {"rank", answers[i][0].second}
                    };
                }
            }
        }

        file << result.dump(4);
        std::cout << "\033[1;32mData successfully written to answers.json\033[0m" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31mError writing answers: " << e.what() << "\033[0m" << std::endl;
    }
}
