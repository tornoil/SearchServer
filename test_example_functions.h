#pragma once
#include "search_server.h"

#include "log_duration.h"
#include "process_queries.h"
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

string GenerateWord(mt19937& generator, int max_length) {
    const int length = uniform_int_distribution(1, max_length)(generator);
    string word;
    word.reserve(length);
    for (int i = 0; i < length; ++i) {
        word.push_back(uniform_int_distribution<int>('a', 'z')(generator));
    }
    return word;
}

vector<string> GenerateDictionary(mt19937& generator, int word_count, int max_length) {
    vector<string> words;
    words.reserve(word_count);
    for (int i = 0; i < word_count; ++i) {
        words.push_back(GenerateWord(generator, max_length));
    }
    sort(words.begin(), words.end());
    words.erase(unique(words.begin(), words.end()), words.end());
    return words;
}

string GenerateQuery(mt19937& generator, const vector<string>& dictionary, int max_word_count) {
    const int word_count = uniform_int_distribution(1, max_word_count)(generator);
    string query;
    for (int i = 0; i < word_count; ++i) {
        if (!query.empty()) {
            query.push_back(' ');
        }
        query += dictionary[uniform_int_distribution<int>(0, dictionary.size() - 1)(generator)];
    }
    return query;
}

vector<string> GenerateQueries(mt19937& generator, const vector<string>& dictionary, int query_count, int max_word_count) {
    vector<string> queries;
    queries.reserve(query_count);
    for (int i = 0; i < query_count; ++i) {
        queries.push_back(GenerateQuery(generator, dictionary, max_word_count));
    }
    return queries;
}

template <typename ExecutionPolicy>
void TestRemoveDocument(string_view mark, SearchServer search_server, ExecutionPolicy&& policy) {
    LOG_DURATION(mark);
    const int document_count = search_server.GetDocumentCount();
    for (int id = 0; id < document_count; ++id) {
        search_server.RemoveDocument(policy, id);
    }
    cout << search_server.GetDocumentCount() << endl;
}

#define TEST_REMOVE_DOCUMENT(mode) TestRemoveDocument(#mode, search_server, execution::mode)

int RunTestRemoveDocument() {
    mt19937 generator;

    const auto dictionary = GenerateDictionary(generator, 10000, 25);
    const auto documents = GenerateQueries(generator, dictionary, 10'000, 100);

    SearchServer search_server(dictionary[0]);
    for (size_t i = 0; i < documents.size(); ++i) {
        search_server.AddDocument(i, documents[i], DocumentStatus::ACTUAL, { 1, 2, 3 });
    }

    TEST_REMOVE_DOCUMENT(seq);
    TEST_REMOVE_DOCUMENT(par);
    return 0;
}

template <typename QueriesProcessor>
void Test(string_view mark, QueriesProcessor processor, const SearchServer& search_server, const vector<string>& queries) {
    LogDuration ld(mark);
    const auto documents_lists = processor(search_server, queries);
}

#define TEST(processor) Test(#processor, processor, search_server, queries)

int TestProcessQueries() {
    mt19937 generator;
    const auto dictionary = GenerateDictionary(generator, 10000, 25);
    const auto documents = GenerateQueries(generator, dictionary, 100'000, 10);

    SearchServer search_server(dictionary[0]);
    for (size_t i = 0; i < documents.size(); ++i) {
        search_server.AddDocument(i, documents[i], DocumentStatus::ACTUAL, {1, 2, 3});
    }

    const auto queries = GenerateQueries(generator, dictionary, 10'000, 7);
    TEST(ProcessQueries);
    return 0;
}

template <typename QueriesProcessor>
void TestQueriesProcessor(string_view mark, QueriesProcessor processor, const SearchServer& search_server, const vector<string>& queries) {
    LOG_DURATION(mark);
    const auto documents = processor(search_server, queries);
    cout << documents.size() << endl;
}

#define TESTQUERIESPROCESSOR(processor) TestQueriesProcessor(#processor, processor, search_server, queries)

int TestProcessQueriesJoined() {
    mt19937 generator;
    const auto dictionary = GenerateDictionary(generator, 10000, 25);
    const auto documents = GenerateQueries(generator, dictionary, 100'000, 10);

    SearchServer search_server(dictionary[0]);
    for (size_t i = 0; i < documents.size(); ++i) {
        search_server.AddDocument(i, documents[i], DocumentStatus::ACTUAL, {1, 2, 3});
    }

    const auto queries = GenerateQueries(generator, dictionary, 10'000, 7);
    TESTQUERIESPROCESSOR(ProcessQueriesJoined);
    return 0;
}

int TestMatch() {
    SearchServer search_server("and with"s);

    int id = 0;
    for (
        const string& text : {
            "funny pet and nasty rat"s,
            "funny pet with curly hair"s,
            "funny pet and not very nasty rat"s,
            "pet with rat and rat and rat"s,
            "nasty rat with curly hair"s,
        }
    ) {
        search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, {1, 2});
    }

    const string query = "curly and funny -not"s;

    {
        const auto [words, status] = search_server.MatchDocument(query, 1);
        cout << words.size() << " words for document 1"s << endl;
        // 1 words for document 1
    }

    {
        const auto [words, status] = search_server.MatchDocument(execution::seq, query, 2);
        cout << words.size() << " words for document 2"s << endl;
        // 2 words for document 2
    }

    {
        const auto [words, status] = search_server.MatchDocument(execution::par, query, 3);
        cout << words.size() << " words for document 3"s << endl;
        // 0 words for document 3
    }

    return 0;
}

template <typename ExecutionPolicy>
void Test(string_view mark, const SearchServer& search_server, const vector<string>& queries, ExecutionPolicy&& policy) {
    LOG_DURATION(mark);
    double total_relevance = 0;
    for (const string_view query : queries) {
        for (const auto& document : search_server.FindTopDocuments(policy, query)) {
            total_relevance += document.relevance;
        }
    }
    cout << total_relevance << endl;
}

#define TEST_FIND_TOP_DOC(policy) Test(#policy, search_server, queries, execution::policy)

int TestFindTopDocument() {
    mt19937 generator;

    const auto dictionary = GenerateDictionary(generator, 1000, 10);
    const auto documents = GenerateQueries(generator, dictionary, 10'000, 70);

    SearchServer search_server(dictionary[0]);
    for (size_t i = 0; i < documents.size(); ++i) {
        search_server.AddDocument(i, documents[i], DocumentStatus::ACTUAL, {1, 2, 3});
    }

    const auto queries = GenerateQueries(generator, dictionary, 100, 70);

    TEST_FIND_TOP_DOC(seq);
    TEST_FIND_TOP_DOC(par);

    return 0;
}