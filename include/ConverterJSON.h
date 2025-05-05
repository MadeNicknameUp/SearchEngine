#ifndef CONVERTERJSON_H
#define CONVERTERJSON_H

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <utility>

class ConverterJSON {
public:
    std::vector<std::string> GetTextDocuments() const;

    std::vector<std::string> GetRequests() const;

    void putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers) const;

private:
    using json = nlohmann::json;

    mutable json config_cache;
    mutable json requests_cache;
    mutable bool config_loaded = false;
    mutable bool requests_loaded = false;

    void loadConfig() const;
    void loadRequests() const;
    json safeParse(const std::string& filename) const;
};

#endif // CONVERTERJSON_H
