#ifndef INVERTEDINDEX_H
#define INVERTEDINDEX_H

#include "ConverterJSON.h"

#include <unordered_map>
#include <mutex>
#include <string>

class InvertedIndex {
public:
    InvertedIndex() = default;

    void UpdateDocumentBase();

    const std::unordered_map<size_t, size_t>& GetWordCount(const std::string& word) const;
private:
    void ProcessFile(const std::string& files_path, const size_t doc_id);

    std::unordered_map<std::string, std::unordered_map<size_t, size_t>> freq_dictionary;
    std::mutex dict_mutex;
    ConverterJSON converter;
};

#endif // INVERTEDINDEX_H
