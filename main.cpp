#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <chrono>
using std::cin;
using std::cout;
using std::ifstream;
using std::string;
using std::string_view;

int event_count = 8;
string event_type[] = {"LOGIN", "LOGOUT", "TOKEN_REFRESH", "ACCESS", "FAILED_LOGIN", "OPEN_APP", "DOWNLOAD", "ADMIN_ACTION"};
int location_count = 15;
string location_list[] = {"US", "VN", "JP", "KR", "SG", "CN", "DE", "FR", "UK", "AU", "CA", "IN", "BR", "RU", "TH"};

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

string_view nextToken(const char*& p){
    const char* start = p; 
    while (*p != 0 && *p != ','){
        p++;
    }

    size_t length = p - start; 

    if (*p == ',') {
        ++p; 
    }

    return string_view(start, length); 
}

void parseLine(string& line, Log& log){
    const char* line_ptr = line.c_str(); 

    log.user_id = nextToken(line_ptr);
    log.device_id = nextToken(line_ptr);
    log.app_id = nextToken(line_ptr);
    log.resource_id = nextToken(line_ptr);

    string_view event = nextToken(line_ptr); 
    string_view location = nextToken(line_ptr); 
    string_view timestamp = nextToken(line_ptr); 

    for (int i = 0; i < event_count; i++) {
        if (event == event_type[i]) {
            log.event_type_index = i; 
            break; 
        }
    }
    for (int i = 0; i < location_count; i++) {
        if (location == location_list[i]) {
            log.location_index = i; 
            break;
        }
    }
    log.timestamp = std::stoll(string(timestamp)); 

}

void loadData(const string filename, Log *&memory)
{
    ifstream fIn;
    fIn.open(filename);
    if (!fIn.is_open())
    {
        cout << "Khong mo duoc file!\n";
        return;
    }
    string line = "";
    getline(fIn, line);

    int count = 0;
    while (getline(fIn, line))
    {
        parseLine(line, memory[count]);
        count++; 
    }
    fIn.close();
}

int main()
{

    int line_count = 1000000;
    Log* logs = new Log[line_count];

    auto start = std::chrono::high_resolution_clock::now(); 

    loadData("dataset_1m.csv", logs);
    cout << "Load len RAM thanh cong\n"; 

    auto end = std::chrono::high_resolution_clock::now(); 

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start); 

    cout << "Thoi gian: " << duration.count() << "ms" << "\n"; 

    delete[] logs;
    return 0;
}