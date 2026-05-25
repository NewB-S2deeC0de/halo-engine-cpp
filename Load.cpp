#include <iostream>
#include <charconv>
#include <cstring>
#include <cstdio>
using std::cout;

#include "Load.h"

string_view nextToken(const char *&p, const char *end)
{
    const char *start = p;
    while (p < end && (*p == '\r' || *p == '\n')) {
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

int lookupLocation(string_view sv)
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

int lookupEvent(string_view sv)
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

bool parseLine(const char *&p, const char *end, Log &log)
{
    if (p >= end)
    {
        return false;
    }

    log.user_id = nextToken(p, end);
    log.device_id = nextToken(p, end);
    log.app_id = nextToken(p, end);
    log.resource_id = nextToken(p, end);

    string_view event = nextToken(p, end);
    string_view location = nextToken(p, end);
    string_view timestamp = nextToken(p, end);

    log.location_index = lookupLocation(location);
    log.event_type_index = lookupEvent(event);
    long long ts = 0;
    std::from_chars(timestamp.data(), timestamp.data() + timestamp.size(), ts);
    log.timestamp = ts;
    return true;
}

void loadData(const char *filename, Log *&logs, HashTable &ht_user, HashTable &ht_resource, char *&buf, int &count)
{
    FILE *f = fopen(filename, "rb");
    if (f == nullptr)
    {
        cout << "fopen failed: " << strerror(errno) << "\n";
        return;
    }
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    buf = new char[file_size];
    size_t bytes = fread(buf, sizeof(char), file_size, f);
    fclose(f);

    const char *p = buf;
    const char *end = buf + bytes;

    while (p < end && *p != '\n')
    {
        p++;
    }
    if (p < end)
    {
        p++;
    }

    count = 0;
    while (count < line_count && parseLine(p, end, logs[count]))
    {
        int user_id_hash = hashing(logs[count].user_id);
        int start_hash = user_id_hash; 
        while (ht_user.node[user_id_hash].head_log_index != -1)
        {
            // neu nhu la trung user
            if (logs[count].user_id == ht_user.node[user_id_hash].key)
            {
                logs[count].previous_user_log = ht_user.node[user_id_hash].head_log_index; // cap nhat lai log trung user truoc log nay
                ht_user.node[user_id_hash].head_log_index = count;                         // cap nhat lai log moi nhat trung user
                break;
            }
            user_id_hash = (user_id_hash + 1) % capacity;
            if (user_id_hash == start_hash) {
                cout << "loi: bang user da het cho\n";
            }
        }
        if (ht_user.node[user_id_hash].head_log_index == -1)
        {
            logs[count].previous_user_log = -1;                // khong co log nao trung user truoc log nay
            ht_user.node[user_id_hash].head_log_index = count; // logs[0] la log moi nhat trung user nay
            ht_user.node[user_id_hash].key = logs[count].user_id;
            ht_user.bucket_used++;
        }

        int resource_id_hash = hashing(logs[count].resource_id);
        start_hash = resource_id_hash; 
        while (ht_resource.node[resource_id_hash].head_log_index != -1)
        {
            if (ht_resource.node[resource_id_hash].key == logs[count].resource_id)
            {
                logs[count].previous_resource_log = ht_resource.node[resource_id_hash].head_log_index;
                ht_resource.node[resource_id_hash].head_log_index = count;
                break;
            }
            resource_id_hash = (resource_id_hash + 1) % capacity; 
            if (resource_id_hash == start_hash) {
                cout << "loi: bang resource da het cho\n";
            }
        }
        if (ht_resource.node[resource_id_hash].head_log_index == -1) {
            ht_resource.node[resource_id_hash].key = logs[count].resource_id;
            logs[count].previous_resource_log = -1; 
            ht_resource.node[resource_id_hash].head_log_index = count;
            ht_resource.bucket_used++;
        }

        count++;
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

int hashing(string_view user_id)
{
    return hash_fnv1a(user_id.data(), user_id.size()) % capacity;
}

int load_factor(HashTable ht)
{
    return ht.bucket_used / capacity;
}