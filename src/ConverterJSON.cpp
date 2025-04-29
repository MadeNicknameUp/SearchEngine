#include "ConverterJSON.h"

#include <fstream>
#include <iostream>

using std::runtime_error;
using std::exception;
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;
using std::pair;
using std::cerr;
using std::cout;
using std::endl;
using std::ios;

void ConverterJSON::loadConfig() const {
    if (!config_loaded) {
        config_cache = safeParse("config.json");
        config_loaded = true;
    }
}

void ConverterJSON::loadRequests() const {
    if (!requests_loaded) {
        requests_cache = safeParse("requests.json");
        requests_loaded = true;
    }
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

    if (!config_cache.contains("files")) {
        throw runtime_error("Field 'files' not found in config.json");
    }

    return config_cache["files"].get<vector<string>>();
}

int ConverterJSON::GetResponsesLimit() const {
    loadConfig();

    if (!config_cache.contains("config")) {
        throw runtime_error("Field 'config' not found in config.json");
    }

    return config_cache["config"]["max_responses"].get<int>();
}

vector<string> ConverterJSON::GetRequests() const {
    loadRequests();

    if (!requests_cache.contains("requests")) {
        throw runtime_error("Field 'requests' not found in requests.json");
    }

    return requests_cache["requests"].get<vector<string>>();
}

void ConverterJSON::putAnswers(const vector<vector<pair<int, float>>>& answers) const {
    ofstream file("answers.json", ios::trunc);

    if (!file) {
        cerr << "\033[1;31mError: Can't open answers.json\033[0m" << endl;
        return;
    }

    try {
        json data = answers;
        file << data.dump(4);
        cout << "\033[1;32mData successfully written to answers.json\033[0m" << endl;
    } catch (const exception& e) {
        cerr << "\033[1;31mError writing answers: " << e.what() << "\033[0m" << endl;
    }
}
