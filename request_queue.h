#pragma once
#include "search_server.h"
#include <deque>

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server):search_server_(search_server) {
        
    }
    // сделаем "обёртки" для всех методов поиска, чтобы сохранять результаты для нашей статистики
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
        
        if(requests_.size() == sec_in_day_){
            requests_.pop_front();
        }
        std::vector<Document> result = search_server_.FindTopDocuments(raw_query,document_predicate);
        requests_.push_back({raw_query,result});
        return result;
    }

    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    std::vector<Document> AddFindRequest(const std::string& raw_query);
    int GetNoResultRequests() const;
    void AddRequest(const std::string& raw_query,const std::vector<Document>& result);
private:
    struct QueryResult {
        const std::string query;
        std::vector<Document> result;
    };
    std::deque<QueryResult> requests_;
    const static int sec_in_day_ = 1440;
    const SearchServer& search_server_;
};