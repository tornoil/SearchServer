#pragma once
#include <vector>
#include <list>
#include <algorithm>
#include <execution>
#include "search_server.h"
#include "document.h"

std::vector<std::vector<Document>> ProcessQueries(const SearchServer& search_server, const std::vector<std::string>& queries) {
    std::vector<std::vector<Document>> result(queries.size());
    std::transform(std::execution::par, queries.begin(), queries.end(), result.begin(), [&search_server](std::string_view query) {return search_server.FindTopDocuments(query); });

    return result;
}

std::vector<Document> ProcessQueriesJoined(const SearchServer& search_server,
    const std::vector<std::string>& queries) {
    std::vector<Document> result;
    for (auto& documents : ProcessQueries(search_server, queries)) {
        std::transform(documents.begin(), documents.end(), std::back_inserter(result),
            [](auto& document) {
                return std::move(document);
            });
    }
    return result;
}