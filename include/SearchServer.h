#include "InvertedIndex.h"

#include <vector>

struct RelativeIndex {
    size_t doc_id;
    float rank;

    bool operator==(const RelativeIndex& other) const {
        return (doc_id == other.doc_id && rank == other.rank);
    }
};

class SearchServer {
public:
    SearchServer(InvertedIndex& idx) : _index(idx) {}

    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& input_requests);

private:
    InvertedIndex& _index;

    std::vector<std::string> processQuery(const std::string& request);
    std::vector<size_t> findMatchingDocs(const std::vector<std::string>& words);
    std::vector<RelativeIndex> rankDocuments(
        const std::vector<size_t>& doc_ids,
        const std::vector<std::string>& words
    );
};
