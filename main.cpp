#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <chrono>
#include <charconv>
using std::cin;
using std::cout;
using std::ifstream;
using std::string;
using std::string_view;

int event_count = 8;
string event_type[] = {"LOGIN", "LOGOUT", "TOKEN_REFRESH", "ACCESS", "FAILED_LOGIN", "OPEN_APP", "DOWNLOAD", "ADMIN_ACTION"};
int location_count = 15;
string location_list[] = {"US", "VN", "JP", "KR", "SG", "CN", "DE", "FR", "UK", "AU", "CA", "IN", "BR", "RU", "TH"};
int line_count = 1000000;
struct Log
{
    string_view user_id;
    string_view device_id;
    string_view app_id;
    string_view resource_id;
    int event_type_index;
    int location_index;
    long long timestamp;
};

string_view nextToken(const char *&p, const char *end)
{
    const char *start = p;
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
    uint16_t key = ((uint16_t)sv[0] << 8) | sv[1];
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

void loadData(const char *filename, Log *&memory, char *&buf, int& count)
{
    FILE *f = fopen(filename, "rb");
    if (f == nullptr)
    {
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
    while (count < line_count && parseLine(p, end, memory[count]))
    {
        count++;
    }
}

int main()
{

    Log *logs = new Log[line_count];
    char *buffer = nullptr;
    int count = 0;

    auto start = std::chrono::high_resolution_clock::now();

    loadData("dataset_1m.csv", logs, buffer, count);
    cout << "Load len RAM thanh cong\n";

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    cout << "Thoi gian: " << duration.count() << "ms" << "\n";

    delete[] logs;
    delete[] buffer;
    return 0;
}