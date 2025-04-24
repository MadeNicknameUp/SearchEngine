#include "ConverterJSON.h"

std::vector<std::string> ConverterJSON::GetTextDocuments() const
{
    std::ifstream input("config.json");

    if (!input.is_open()) {
        throw std::runtime_error("Could not open config.json");
    }

    json data;
    input >> data;

    if (!data.contains("files")) {
        throw std::runtime_error("Field 'files' not found in JSON");
    }

    return data["files"].get<std::vector<std::string>>();
}

int ConverterJSON::GetResponsesLimit() const
{
    std::ifstream input("config.json");

    if (!input.is_open()) {
        throw std::runtime_error("Could not open config.json");
    }

    json data;
    input >> data;

    if (!data.contains("config")) {
        throw std::runtime_error("Field 'config' not found in JSON");
    }

    return data["config"]["max_responses"];
}

std::vector<std::string> ConverterJSON::GetRequests() const
{
    std::ifstream input("requests.json");

    if (!input.is_open()) {
        throw std::runtime_error("Could not open requests.json");
    }

    json data;
    input >> data;

    if (!data.contains("requests")) {
        throw std::runtime_error("Field 'requests' not found in JSON");
    }

    return data["requests"].get<std::vector<std::string>>();
}

void ConverterJSON::putAnswers(std::vector<std::vector<std::pair<int, float>>> answer) const
{
    std::ofstream file("answers.json");

    if (file.is_open())
    {
        json data = answer;
        file << data.dump(4);
        file.close();
        std::cout << "\e[1;32m Data has been written in answers.json successfully \e[0m" << std::endl;
    }
    else {
        std::cerr << "\e[1;31m Error: Can't open answers.json \e[m"<< std::endl;
    }
}
