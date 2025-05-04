#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"
#include <iostream>

int main() {
    try {
        ConverterJSON converter;
        InvertedIndex index;
        SearchServer server(index);

        std::cout << "Starting SearchEngine..." << std::endl;

        std::cout << "Indexing documents..." << std::endl;
        index.UpdateDocumentBase();

        std::cout << "Processing requests..." << std::endl;
        auto requests = converter.GetRequests();

        auto results = server.search(requests);

        std::cout << "Saving results..." << std::endl;

        std::vector<std::vector<std::pair<int, float>>> converted_results;

        for (const auto& query_results : results) {
            std::vector<std::pair<int, float>> converted_query;
            for (const auto& rel_index : query_results) {
                converted_query.emplace_back(rel_index.doc_id, rel_index.rank);
            }
            converted_results.push_back(converted_query);
        }

        converter.putAnswers(converted_results);

        std::cout << "Search completed. Results saved to answers.json" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
