#include "request_queue.h"
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {           
    std::vector<Document> result = search_server_.FindTopDocuments(std::execution::par, raw_query,status);
    AddRequest(raw_query,result);
    return result;
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {       
    std::vector<Document> result = search_server_.FindTopDocuments(raw_query);
    AddRequest(raw_query,result);
    return result;
}

int RequestQueue::GetNoResultRequests() const {        
    int no_results = 0;
    for(const QueryResult& request: requests_){
        if(request.result.empty()){
            ++no_results;
        }
    }
    return no_results;
}
void RequestQueue::AddRequest(const std::string& raw_query,const std::vector<Document>& result){
    if(requests_.size() == sec_in_day_){
        requests_.pop_front();
    }
    requests_.push_back({raw_query,result});
}