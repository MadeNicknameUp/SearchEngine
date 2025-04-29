#include "InvertedIndex.h"
#include "SearchServer.h"

#include <algorithm>
#include <cmath>
#include <unordered_set>

struct RelativeIndex {
    size_t doc_id;
    float rank;

    bool operator ==(const RelativeIndex& other) const {
        return (doc_id == other.doc_id && rank == other.rank);
    }
};

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input) {
    std::vector<std::vector<RelativeIndex>> result;

    for (const auto& query : queries_input) {
        // 1. Разбиваем запрос на слова
        auto words = splitQuery(query);
        if (words.empty()) {
            result.push_back({});
            continue;
        }

        // 2. Получаем уникальные слова и сортируем по частоте
        auto sorted_words = getSortedUniqueWords(words);

        // 3. Находим документы, содержащие все слова
        auto docs = findRelevantDocs(sorted_words);

        // 4. Рассчитываем релевантность
        auto ranked_docs = calculateRank(docs, sorted_words);

        result.push_back(ranked_docs);
    }

    return result;
}

std::vector<std::string> SearchServer::splitQuery(const std::string& query) {
    std::vector<std::string> words;
    std::istringstream iss(query);
    std::string word;

    while (iss >> word) {
        words.push_back(word);
    }

        return words;
}

std::vector<std::string> SearchServer::getSortedUniqueWords(const std::vector<std::string>& words) {
    std::unordered_set<std::string> unique_words(words.begin(), words.end());
    std::vector<std::string> sorted_words(unique_words.begin(), unique_words.end());

    // Сортируем по возрастанию частоты (самые редкие сначала)
    std::sort(sorted_words.begin(), sorted_words.end(),
        [this](const std::string& a, const std::string& b) {
            auto count_a = getTotalWordCount(a);
            auto count_b = getTotalWordCount(b);
            return count_a < count_b;
        });

    return sorted_words;
}

size_t SearchServer::getTotalWordCount(const std::string& word) {
    size_t total = 0;
    auto entries = _index.GetWordCount(word);
    for (const auto& [doc_id, count] : entries) {
        total += count;
    }
    return total;
}

// 3. Поиск документов, содержащих все слова
std::vector<size_t> SearchServer::findRelevantDocs(const std::vector<std::string>& sorted_words) {
    if (sorted_words.empty()) return {};

    // Начинаем с документов для самого редкого слова
    auto initial_docs = _index.GetWordCount(sorted_words[0]);
    std::vector<size_t> docs;
    for (const auto& [doc_id, _] : initial_docs) {
        docs.push_back(doc_id);
    }

    // Проверяем остальные слова
    for (size_t i = 1; i < sorted_words.size(); ++i) {
        auto word_docs = _index.GetWordCount(sorted_words[i]);
        std::vector<size_t> new_docs;

        for (auto doc_id : docs) {
            if (word_docs.count(doc_id)) {
                new_docs.push_back(doc_id);
            }
        }

        if (new_docs.empty()) return {};
        docs = std::move(new_docs);
    }

    return docs;
}

std::vector<RelativeIndex> calculateRank(const std::vector<size_t>& docs,
                                        const std::vector<std::string>& words) {
    if (docs.empty()) return {};

    std::vector<RelativeIndex> result;
    float max_rank = 0;

    // Рассчитываем абсолютную релевантность
    for (auto doc_id : docs) {
        float abs_rank = 0;
        for (const auto& word : words) {
            auto entries = _index.GetWordCount(word);
            if (entries.count(doc_id)) {
                abs_rank += entries.at(doc_id);
            }
        }

        result.push_back({doc_id, abs_rank});
        max_rank = std::max(max_rank, abs_rank);
    }

    if (max_rank > 0) {
        for (auto& item : result) {
            item.rank /= max_rank;
        }
    }

    std::sort(result.begin(), result.end(),
        [](const RelativeIndex& a, const RelativeIndex& b) {
            return a.rank > b.rank;
        });

    return result;
}
