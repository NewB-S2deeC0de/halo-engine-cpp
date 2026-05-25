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

    HashTable table_user;
    HashTable table_resource; 
    table_user.node = new Node[capacity];
    table_resource.node = new Node[capacity];

    auto start = std::chrono::high_resolution_clock::now();

    loadData("dataset_1m.csv", log_list, table_user, table_resource, buffer, cnt);
    if (cnt == 1000000) {
        cout << "Load len RAM thanh cong\n";
    }


    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    cout << "Thoi gian: " << duration.count() << "ms" << "\n";

    delete[] table_resource.node; 
    delete[] table_user.node;
    delete[] log_list;
    delete[] buffer;
    
    return 0;
}