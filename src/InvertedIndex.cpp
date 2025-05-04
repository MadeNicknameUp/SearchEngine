#include "InvertedIndex.h"

#include <fstream>
#include <thread>
#include <vector>

void InvertedIndex::UpdateDocumentBase() {
    {
        std::lock_guard<std::mutex> lock(dict_mutex);
        freq_dictionary.clear();
    }

    std::vector<std::string> files_paths;
    std::vector<std::thread> threads;

    files_paths = converter.GetTextDocuments();

    for (size_t doc_id = 0; doc_id < files_paths.size(); ++doc_id) {
        threads.emplace_back(
            &InvertedIndex::ProcessFile, this,
            std::cref(files_paths[doc_id]), doc_id
        );
    }

    for (auto& thread : threads) {
        if (thread.joinable()) thread.join();
    }
}

void InvertedIndex::ProcessFile(const std::string& files_path, const size_t doc_id) {
    std::ifstream file(files_path);
    if (!file.is_open()) return;

    std::string word;
    while (file >> word) {
        std::lock_guard<std::mutex> lock(dict_mutex);
        freq_dictionary[word][doc_id]++;
    }
    file.close();
}

const std::unordered_map<size_t, size_t>& InvertedIndex::GetWordCount(const std::string& word) const {
    static const std::unordered_map<size_t, size_t> empty_result;

    auto it = freq_dictionary.find(word);
    return (it != freq_dictionary.end()) ? it->second : empty_result;
}
