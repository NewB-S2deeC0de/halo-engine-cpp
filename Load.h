#ifndef _LOAD_H_
#define _LOAD_H_

#include <iostream>
#include <string> 
using std::string;
using std::string_view;

struct Log
{
    string_view user_id;
    string_view device_id;
    string_view app_id;
    string_view resource_id;
    int event_type_index;
    int location_index;
    long long timestamp;
    
    int previous_user_log = -1;    // log truoc do co cung key
    int previous_resource_log = -1; 
};

const int event_count = 8;
const string event_type[] = {"LOGIN", "LOGOUT", "TOKEN_REFRESH", "ACCESS", "FAILED_LOGIN", "OPEN_APP", "DOWNLOAD", "ADMIN_ACTION"};
const int location_count = 15;
const string location_list[] = {"US", "VN", "JP", "KR", "SG", "CN", "DE", "FR", "UK", "AU", "CA", "IN", "BR", "RU", "TH"};
const int line_count = 1000000;

struct Node
{
    string_view key;
    int head_log_index = -1;    // log moi nhat co cung key
};

struct HashTable
{
    Node* node = nullptr; 
    int bucket_used = 0; // số phần tử lưu trong bảng thực tế
};

// capactity = n / load_factor mong muon = 100,000 / 0.7 = 142,857
// so nguyen to gan nhat la 142,867 hoac 1429 cho 1000 user
const int capacity = 142867;

string_view nextToken(const char *&p, const char *end);
int lookupLocation(string_view sv);
int lookupEvent(string_view sv);
bool parseLine(const char *&p, const char *end, Log &log);
void loadData(const char *filename, Log *&logs, HashTable& ht_user, HashTable& ht_resource, char *&buf, int &count);

unsigned long long hash_fnv1a(const char *str, size_t len);
int hashing(string_view key);
int load_factor(HashTable ht);

#endif