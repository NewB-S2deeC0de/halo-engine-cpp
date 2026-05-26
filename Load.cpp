#include <iostream>
#include <charconv>
#include <cstring>
#include <cstdio>
using std::cout;

#include "Load.h"

void StringDict::init(int cap, int pool_size)
{
    {
        capacity = cap;
        nodes = new Node[capacity];
        string_pool = new char[pool_size];
    }
}

int32_t StringDict::getOrAdd(string_view sv, int count, int32_t &previous_log, int capacity)
{
    int idx = hashing(sv, capacity);
    int start_idx = idx; 

    while (nodes[idx].id != -1)
    {
        if (nodes[idx].key == sv)
        {
            previous_log = nodes[idx].head_log_index;
            nodes[idx].head_log_index = count;
            return nodes[idx].id;
        }
        idx = (idx + 1) % capacity;
        if (idx == start_idx) {
            cout << "Bang user da day, khong the chen them\n"; 
            break;
        }
    }

    char *str = string_pool + pool_offset;
    memcpy(str, sv.data(), sv.size());
    pool_offset += sv.size();

    nodes[idx].key = string_view(str, sv.size());
    nodes[idx].id = current_id;
    nodes[idx].head_log_index = count;

    current_id++;
    return nodes[idx].id;
}

int32_t StringDict::getOrAddSimple(string_view sv, int capacity)
{
    int idx = hashing(sv, capacity);
    int start_idx = idx; 
    while (nodes[idx].id != -1)
    {
        if (nodes[idx].key == sv)
        {
            return nodes[idx].id;
        }
        idx = (idx + 1) % capacity;
        if (idx == start_idx) {
            cout << "bang app da day\n"; 
            break;
        }
    }

    char *str = string_pool + pool_offset;
    memcpy(str, sv.data(), sv.size());
    pool_offset += sv.size();

    nodes[idx].key = string_view(str, sv.size());
    nodes[idx].id = current_id;

    current_id++;
    return nodes[idx].id;
}

string_view nextToken(const char *&p, const char *end)
{
    const char *start = p;
    while (p < end && (*p == '\r' || *p == '\n'))
    {
        p++;
    }
    while (p < end && *p != '\n' && *p != '\r' && *p != ',')
    {
        p++;
    }

    size_t length = p - start;

    if (p < end && *p == '\r')
    {
        ++p;
    }
    if (p < end && *p == '\n')
    {
        ++p;
    }
    if (p < end && *p == ',')
    {
        ++p;
    }
    return string_view(start, length);
}

int8_t lookupLocation(string_view sv)
{
    if (sv.size() < 2)
    {
        return -1;
    }
    u_int16_t key = ((u_int16_t)sv[0] << 8) | sv[1];
    switch (key)
    {
    case ('U' << 8 | 'S'):
    {
        return 0;
    }
    case ('V' << 8 | 'N'):
    {
        return 1;
    }
    case ('J' << 8 | 'P'):
    {
        return 2;
    }
    case ('K' << 8 | 'R'):
    {
        return 3;
    }
    case ('S' << 8 | 'G'):
    {
        return 4;
    }
    case ('C' << 8 | 'N'):
    {
        return 5;
    }
    //  "CA", "IN", "BR", "RU", "TH"};
    case ('D' << 8 | 'E'):
    {
        return 6;
    }
    case ('F' << 8 | 'R'):
    {
        return 7;
    }
    case ('U' << 8 | 'K'):
    {
        return 8;
    }
    case ('A' << 8 | 'U'):
    {
        return 9;
    }
    case ('C' << 8 | 'A'):
    {
        return 10;
    }
    case ('I' << 8 | 'N'):
    {
        return 11;
    }
    case ('B' << 8 | 'R'):
    {
        return 12;
    }
    case ('R' << 8 | 'U'):
    {
        return 13;
    }
    case ('T' << 8 | 'H'):
    {
        return 14;
    }
    default:
    {
        return -1;
    }
    }
}

int8_t lookupEvent(string_view sv)
{
    switch (sv.size())
    {
    case 5:
    {
        return 0;
    }
    case 6:
    {
        return sv[0] == 'L' ? 1 : 3;
    }
    case 8:
    {
        return sv[0] == 'O' ? 5 : 6;
    }
    case 12:
    {
        return sv[0] == 'F' ? 4 : 7;
    }
    case 13:
    {
        return 2;
    }
    default:
    {
        return -1;
    }
    }
}

void loadData(const char *filename, Log *&logs, StringDict &users, StringDict &devices, StringDict &apps, StringDict &resources, char *&buf, int &count)
{
    FILE *f = fopen(filename, "rb");
    if (f == nullptr)
    {
        return;
    }

    int character; 
    while ((character = fgetc(f)) != EOF && character != '\n') {
        //
    }

    const int CHUNK_SIZE = 50 * 1024 * 1024;
    buf = new char[CHUNK_SIZE];
    int leftover_bytes = 0;

    count = 0;
    while (true)
    {
        size_t bytes_read = fread(buf + leftover_bytes, 1, CHUNK_SIZE - leftover_bytes, f);
        if (bytes_read == 0 && leftover_bytes == 0)
        {
            break;
        }

        size_t valid_bytes = leftover_bytes + bytes_read;
        const char *p = buf;
        const char *end = buf + valid_bytes;

        const char *safe_end = end - 1;
        if (bytes_read == CHUNK_SIZE - leftover_bytes)
        {
            while (safe_end > buf && *safe_end != '\n')
            {
                safe_end--;
            }
            safe_end++;
        }
        else
        {
            safe_end = end;
        }

        while (p < safe_end)
        {
            while (p < safe_end && (*p == '\n' || *p == '\r'))
            {
                p++;
            }
            if (p >= safe_end)
            {
                break;
            }

            string_view user_sv = nextToken(p, end);
            string_view device_sv = nextToken(p, end);
            string_view app_sv = nextToken(p, end);
            string_view resource_sv = nextToken(p, end);
            string_view event = nextToken(p, end);
            string_view location = nextToken(p, end);
            string_view timestamp = nextToken(p, end);

            logs[count].user_id = users.getOrAdd(user_sv, count, logs[count].previous_user_log, CAPACITY);
            logs[count].resource_id = resources.getOrAdd(resource_sv, count, logs[count].previous_resource_log, CAPACITY);

            logs[count].device_id = devices.getOrAddSimple(device_sv, CAPACITY);
            logs[count].app_id = apps.getOrAddSimple(app_sv, APP_CAPACITY);

            logs[count].event_type_index = lookupEvent(event);
            logs[count].location_index = lookupLocation(location);

            long long ts = 0;
            std::from_chars(timestamp.data(), timestamp.data() + timestamp.size(), ts);
            logs[count].timestamp = ts;

            count++;
        }

        leftover_bytes = end - safe_end; 
        if (leftover_bytes > 0) {
            memmove(buf, safe_end, leftover_bytes);
        }
    }
}

unsigned long long hash_fnv1a(const char *str, size_t len)
{
    unsigned long long hash = 14695981039346656037ULL;
    for (size_t i = 0; i < len; i++)
    {
        hash ^= (unsigned char)str[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

int hashing(string_view user_id, int capacity)
{
    return hash_fnv1a(user_id.data(), user_id.size()) % capacity;
}