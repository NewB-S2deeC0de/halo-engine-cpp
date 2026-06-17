#ifndef _LOAD_H_
#define _LOAD_H_

#include <iostream>
#include <cstdint>
#include <string>
using std::string;
using std::string_view;

struct Log
{
    int32_t user_id;
    int32_t device_id;
    int32_t app_id;
    int32_t resource_id;
    int8_t event_type_index;
    int8_t location_index;
    long long timestamp;

    int32_t next_user_log = -1; // log truoc do co cung key
    int32_t next_resource_log = -1;
    int32_t next_device_log = -1;
};

const int event_count = 8;
const string event_type[] = {"LOGIN", "LOGOUT", "TOKEN_REFRESH", "ACCESS", "FAILED_LOGIN", "OPEN_APP", "DOWNLOAD", "ADMIN_ACTION"};
const int location_count = 15;
const string location_list[] = {"US", "VN", "JP", "KR", "SG", "CN", "DE", "FR", "UK", "AU", "CA", "IN", "BR", "RU", "TH"};
const int line_count = 1000000;

unsigned long long hash_fnv1a(const char *str, size_t len);
int hashing(string_view key, int capacity);
struct Node
{
    string_view key;
    int32_t id = -1;
    int32_t head_log_index = -1; // log moi nhat co cung key
};

struct StringDict
{
    Node *nodes = nullptr;
    int capacity = 0;
    int32_t current_id = 0;

    char *string_pool = nullptr;
    int pool_offset = 0;

    string_view *id_to_string = nullptr;

    void init(int cap, int pool_size);
    int32_t getOrAdd(string_view sv, int count, int32_t &previous_log, int capacity);
    int32_t getOrAddSimple(string_view sv, int capacity);

    int find(string_view sv) const;
};

// capactity = n / load_factor mong muon = 100,000 / 0.7 = 142,857
// so nguyen to gan nhat la 142,867 hoac 1429 cho 1000 user
const int CAPACITY = 142867;
const int APP_CAPACITY = 14287;

string_view nextToken(const char *&p, const char *end);
int8_t lookupLocation(string_view sv);
int8_t lookupEvent(string_view sv);
bool parseLine(const char *&p, const char *end, Log &log);
void loadData(const char *filename, Log *&logs, StringDict &users, StringDict &devices, StringDict &apps, StringDict &resources, char *&buf, int &count, long long ts_now);

void quickSort(int32_t *indices, int low, int high, const Log *logs);

void rebuildUserChains(StringDict &dict, Log *logs, int line_count);
void rebuildResourceChains(StringDict &dict, Log *logs, int line_count);
void rebuildDeviceChains(StringDict &dict, Log* logs, int line_count);

void queryByUserID(string_view id, long long t1, long long t2,
                   const StringDict &users, const StringDict &devices,
                   const StringDict &apps, const StringDict &resources,
                   const Log *logs);
void queryByResourceID(string_view id, long long t1, long long t2,
                       const StringDict &users, const StringDict &devices,
                       const StringDict &apps, const StringDict &resources,
                       const Log *logs);

struct TopResource
{
    int32_t id;
    int count;
    int32_t max_ts_log;
};

void queryTop10Resources(long long t1, long long t2,
                         const StringDict &resources, Log *logs);

void detectConsecutiveFailedLogins(const StringDict &users, const Log *logs, 
                                    const char* out_filename);
void detectMultipleDevicesLogin(const StringDict &users, const Log *logs, 
                                const char* out_filename);
void detectAbnormalResourceAccess(const StringDict &devices, const Log *logs, 
                                const char* out_filename);
void detectOutsideWorkingHours(const StringDict &users, const Log *logs, 
                                    const char* out_filename);
#endif