#include <chrono>
#include "Load.h"
// #include "Load.cpp"

using std::cin;
using std::cout;
using std::ifstream;

int main()
{
    Log *log_list = new Log[line_count];
    char *buffer = nullptr;
    int cnt = 0;

    StringDict user_dict;
    StringDict device_dict;
    StringDict app_dict;
    StringDict resource_dict;

    user_dict.init(CAPACITY, 2 * 1024 * 1024);
    device_dict.init(CAPACITY, 2 * 1024 * 1024);
    app_dict.init(APP_CAPACITY, 2 * 1024 * 1024);
    resource_dict.init(CAPACITY, 2 * 1024 * 1024);

    auto start = std::chrono::high_resolution_clock::now();

    loadData("dataset_1m.csv", log_list, user_dict, device_dict, app_dict, resource_dict, buffer, cnt);
    if (cnt == 1000000) {
        cout << "Load len RAM thanh cong\n";
    }


    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    cout << "Thoi gian: " << duration.count() << "ms" << "\n";

    delete[] user_dict.nodes;
    delete[] user_dict.string_pool;

    delete[] device_dict.nodes; 
    delete[] device_dict.string_pool;

    delete[] app_dict.nodes; 
    delete[] app_dict.string_pool; 

    delete[] resource_dict.nodes;
    delete[] resource_dict.string_pool; 

    delete[] log_list;
    delete[] buffer;
    
    return 0;
}