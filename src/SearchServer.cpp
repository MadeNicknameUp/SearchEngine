#include "SearchServer.h"

#include <vector>
#include <unordered_set>
#include <sstream>

std::vector<std::string> SearchServer::processQuery(const std::string& query) {
    std::stringstream ss(query);
    std::vector<std::string> words;
    std::string word;

    while (ss >> word) {
        while (!word.empty() && ispunct(word.back())) {
            word.pop_back();
        }
        if (!word.empty()) {
            words.push_back(word);
        }
    }

    std::sort(words.begin(), words.end());
    words.erase(std::unique(words.begin(), words.end()), words.end());

    return words;
}

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& input_requests)
{
    std::vector<std::vector<RelativeIndex>> results;

    _index.UpdateDocumentBase();

    for (const auto& request : input_requests)
    {
        auto words = processQuery(request);

        if (words.empty()) {
            results.emplace_back();
            continue;
        }

        auto doc_ids = findMatchingDocs(words); // here

        if (doc_ids.empty()) {
            results.emplace_back();
            continue;
        }

        results.push_back(rankDocuments(doc_ids, words));
    }

    return results;
}

std::vector<size_t> SearchServer::findMatchingDocs(const std::vector<std::string>& words) {
    if (words.empty()) return {};

    std::unordered_set<size_t> result;

    for (size_t i = 0; i < words.size(); ++i) {
        const auto& word_counts = _index.GetWordCount(words[i]);
        for (const auto& [doc_id, count] : word_counts) {
            result.insert(doc_id);
        }
    }

    return std::vector<size_t>(result.begin(), result.end());
}

std::vector<RelativeIndex> SearchServer::rankDocuments(
    const std::vector<size_t>& doc_ids,
    const std::vector<std::string>& words
) {
    std::vector<RelativeIndex> ranked_docs;
    ranked_docs.reserve(doc_ids.size());
    std::vector<float> abs_ranks;
    abs_ranks.reserve(doc_ids.size());

    for (auto doc_id : doc_ids) {
        float rank = 0;
        for (const auto& word : words) {
            const auto& word_counts = _index.GetWordCount(word);
            auto it = word_counts.find(doc_id);
            if (it != word_counts.end()) {
                rank += it->second;
            }
        }
        abs_ranks.push_back(rank);
    }

    float max_rank = *std::max_element(abs_ranks.begin(), abs_ranks.end());

    for (size_t i = 0; i < doc_ids.size(); ++i) {
        ranked_docs.push_back({
            doc_ids[i],
            max_rank > 0 ? abs_ranks[i] / max_rank : 0
        });
    }

    std::sort(ranked_docs.begin(), ranked_docs.end(),
        [](const RelativeIndex& a, const RelativeIndex& b) {
            return a.rank > b.rank;
        });

    return ranked_docs;
}
