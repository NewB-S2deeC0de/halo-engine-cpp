#include <iostream>
#include <string>
#include <chrono>
#include "Load.h"
// #include "Load.cpp"


using std::cin;
using std::cout;
using std::string;
using std::string_view;

void freeMemory(Log *&log_list, char *&buffer, StringDict &users, StringDict &devices, StringDict &apps, StringDict &resources);

bool initAndLoadData(Log *&log_list, char *&buffer, int &cnt,
                     StringDict &user_dict, StringDict &device_dict,
                     StringDict &app_dict, StringDict &resource_dict,
                     long long timestamp_now);

long long getCurrentEpochTime();

int main()
{
    int choice;
    string input_id;
    long long ts1, ts2;

    while (true)
    {
        cout << "\n=================== HALO CYBER ACCESS ENGINE ===================\n";
        cout << "1. Truy van hanh trinh cua nguoi dung (User ID)\n";
        cout << "2. Truy van hanh trinh cua tai nguyen (Resource ID)\n";
        cout << "3. Thong ke Top 10 tai nguyen duoc truy cap nhieu nhat\n";
        cout << "4. Phat hien bat thuong dua tren nguong (Xuat CSV)\n";
        cout << "0. Thoat chuong trinh\n";
        cout << "================================================================\n";
        cout << "Nhap lua chon cua ban: ";

        if (!(cin >> choice))
        {
            cout << "Ky tu khong hop le. Chuong trinh dang thoat...\n";
            break;
        }

        if (choice == 0)
        {
            cout << "Cam on ban da su dung chuong trinh. Dang thoat...\n";
            break;
        }
        else if (choice == 1)
        {
            Log *log_list = nullptr;
            char *buffer = nullptr;
            int cnt = 0;

            StringDict user_dict;
            StringDict device_dict;
            StringDict app_dict;
            StringDict resource_dict;

            cout << "Nhap User ID can truy van: ";
            cin >> input_id;
            cout << "Nhap timestamp bat dau (ts1): ";
            cin >> ts1;
            cout << "Nhap timestamp ket thuc (ts2): ";
            cin >> ts2;

            string_view sv_id = input_id;

            long long current_ts = getCurrentEpochTime();

            auto start = std::chrono::high_resolution_clock::now();

            bool is_init = initAndLoadData(log_list, buffer, cnt, user_dict, device_dict, app_dict, resource_dict, current_ts);
            if (is_init)
            {
                queryByUserID(sv_id, ts1, ts2, user_dict, device_dict, app_dict, resource_dict, log_list);
            }
            else
            {
                cout << "Loi: khong the tai du lieu tu file\n";
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            cout << "\n[Thoi gian thuc thi]: " << duration.count() << "ms\n";

            freeMemory(log_list, buffer, user_dict, device_dict, app_dict, resource_dict);
            cout << "[He thong] Da thu hoi toan bo bo nho khoi RAM.\n";
        }
        else if (choice == 2)
        {
            Log *log_list = nullptr;
            char *buffer = nullptr;
            int cnt = 0;

            StringDict user_dict;
            StringDict device_dict;
            StringDict app_dict;
            StringDict resource_dict;

            cout << "Nhap Resource ID can truy van: ";
            cin >> input_id;
            cout << "Nhap timestamp bat dau (ts1): ";
            cin >> ts1;
            cout << "Nhap timestamp ket thuc (ts2): ";
            cin >> ts2;

            string_view sv_id = input_id;

            long long current_ts = getCurrentEpochTime();

            auto start = std::chrono::high_resolution_clock::now();

            bool is_init = initAndLoadData(log_list, buffer, cnt, user_dict, device_dict, app_dict, resource_dict, current_ts);
            if (is_init)
            {
                queryByResourceID(sv_id, ts1, ts2, user_dict, device_dict, app_dict, resource_dict, log_list);
            }
            else
            {
                cout << "Loi: khong the tai du lieu tu file\n";
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            cout << "\n[Thoi gian thuc thi]: " << duration.count() << "ms\n";

            freeMemory(log_list, buffer, user_dict, device_dict, app_dict, resource_dict);
            cout << "[He thong] Da thu hoi toan bo bo nho khoi RAM.\n";
        }
        else if (choice == 3)
        {
            Log *log_list = nullptr;
            char *buffer = nullptr;
            int cnt = 0;

            StringDict user_dict;
            StringDict device_dict;
            StringDict app_dict;
            StringDict resource_dict;

            cout << "Nhap timestamp bat dau (ts1): ";
            cin >> ts1;
            cout << "Nhap timestamp ket thuc (ts2): ";
            cin >> ts2;

            long long current_ts = getCurrentEpochTime();

            auto start = std::chrono::high_resolution_clock::now();

            bool is_init = initAndLoadData(log_list, buffer, cnt, user_dict, device_dict, app_dict, resource_dict, current_ts);
            if (is_init)
            {
                queryTop10Resources(ts1, ts2, resource_dict, log_list);
            }
            else {
                cout << "Khong the tai du lieu tu file data\n";
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            cout << "\n[Thoi gian thuc thi]: " << duration.count() << "ms\n";

            freeMemory(log_list, buffer, user_dict, device_dict, app_dict, resource_dict);
            cout << "[He thong] Da thu hoi toan bo bo nho khoi RAM.\n";
        }
        else if (choice == 4)
        {
            int sub_choice;
            while (true)
            {
                cout << "\n--- PHAT HIEN BAT THUONG DUA TREN NGUONG ---\n";
                cout << "1. Nguoi dung dang nhap tu qua nhieu device trong thoi gian ngan\n";
                cout << "2. Nguoi dung login that bai lien tuc\n";
                cout << "3. Mot thiet bi dot ngot truy cap qua nhieu resource khac nhau\n";
                cout << "4. Truy cap ngoai gio lam viec\n";
                cout << "0. Quay lai menu chinh\n";
                cout << "----------------------------------------------\n";
                cout << "Nhap lua chon cua ban: ";

                if (!(cin >> sub_choice))
                {
                    cin.clear();
                    while(cin.get() != '\n'); 
                    cout << "Ky tu khong hop le. Quay lai menu chinh...\n";
                    break;
                }

                if (sub_choice == 0)
                {
                    break; 
                }
                else if (sub_choice == 1)
                {
                    Log *log_list = nullptr;
                    char *buffer = nullptr;
                    int cnt = 0;

                    StringDict user_dict;
                    StringDict device_dict;
                    StringDict app_dict;
                    StringDict resource_dict;

                    auto now = std::chrono::system_clock::now();
                    long long current_ts = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

                    auto start = std::chrono::high_resolution_clock::now();

                    bool is_init = initAndLoadData(log_list, buffer, cnt, user_dict, device_dict, app_dict, resource_dict, current_ts);
                    if (is_init)
                    {
                        detectMultipleDevicesLogin(user_dict, log_list, "multiple_devices_report.csv");
                    }
                    else
                    {
                        cout << "Loi: khong the tai du lieu tu file\n";
                    }

                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                    cout << "\n[Thoi gian thuc thi]: " << duration.count() << "ms\n";

                    freeMemory(log_list, buffer, user_dict, device_dict, app_dict, resource_dict);
                    cout << "[He thong] Da thu hoi toan bo bo nho khoi RAM.\n";
                }
                else if (sub_choice == 2)
                {
                    Log *log_list = nullptr;
                    char *buffer = nullptr;
                    int cnt = 0;

                    StringDict user_dict;
                    StringDict device_dict;
                    StringDict app_dict;
                    StringDict resource_dict;

                    auto now = std::chrono::system_clock::now();
                    long long current_ts = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

                    auto start = std::chrono::high_resolution_clock::now();

                    bool is_init = initAndLoadData(log_list, buffer, cnt, user_dict, device_dict, app_dict, resource_dict, current_ts);
                    if (is_init)
                    {
                        detectConsecutiveFailedLogins(user_dict, log_list, "failed_logins_report.csv");
                    }
                    else
                    {
                        cout << "Loi: khong the tai du lieu tu file\n";
                    }

                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                    cout << "\n[Thoi gian thuc thi]: " << duration.count() << "ms\n";

                    freeMemory(log_list, buffer, user_dict, device_dict, app_dict, resource_dict);
                    cout << "[He thong] Da thu hoi toan bo bo nho khoi RAM.\n";
                }
                else if (sub_choice == 3 || sub_choice == 4)
                {
                    cout << "=> [He thong] Tinh nang dang duoc phat trien...\n";
                }
                else
                {
                    cout << "Lua chon khong hop le.\n";
                }
            }
        }
        else
        {
            cout << "Lua chon khong hop le. Chuong trinh dang thoat...\n";
            break;
        }
    }

    return 0;
}

long long getCurrentEpochTime() {
    auto now = std::chrono::system_clock::now(); 
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
}

bool initAndLoadData(Log *&log_list, char *&buffer, int &cnt,
                     StringDict &user_dict, StringDict &device_dict,
                     StringDict &app_dict, StringDict &resource_dict,
                     long long timestamp_now)
{
    log_list = new Log[line_count];
    buffer = nullptr;
    cnt = 0;

    user_dict.init(CAPACITY, 2 * 1024 * 1024);
    device_dict.init(CAPACITY, 2 * 1024 * 1024);
    app_dict.init(APP_CAPACITY, 2 * 1024 * 1024);
    resource_dict.init(CAPACITY, 2 * 1024 * 1024);

    loadData("dataset_1m.csv", log_list, user_dict, device_dict, app_dict, resource_dict, buffer, cnt, timestamp_now);

    if (cnt > 0)
    {
        rebuildUserChains(user_dict, log_list, cnt);
        rebuildResourceChains(resource_dict, log_list, cnt);
        rebuildDeviceChains(device_dict, log_list, cnt);
        return true;
    }

    cout << "Loi: Khong the tai du lieu tu file!\n";
    return false;
}

void freeMemory(Log *&log_list, char *&buffer, StringDict &users, StringDict &devices, StringDict &apps, StringDict &resources)
{
    if (log_list != nullptr)
    {
        delete[] log_list;
        log_list = nullptr;
    }
    if (buffer != nullptr)
    {
        delete[] buffer;
        buffer = nullptr;
    }

    if (users.nodes != nullptr)
    {
        delete[] users.nodes;
        users.nodes = nullptr;
    }
    if (users.string_pool != nullptr)
    {
        delete[] users.string_pool;
        users.string_pool = nullptr;
    }
    if (users.id_to_string != nullptr)
    {
        delete[] users.id_to_string;
        users.id_to_string = nullptr;
    }

    if (devices.nodes != nullptr)
    {
        delete[] devices.nodes;
        devices.nodes = nullptr;
    }
    if (devices.string_pool != nullptr)
    {
        delete[] devices.string_pool;
        devices.string_pool = nullptr;
    }
    if (devices.id_to_string != nullptr)
    {
        delete[] devices.id_to_string;
        devices.id_to_string = nullptr;
    }

    if (apps.nodes != nullptr)
    {
        delete[] apps.nodes;
        apps.nodes = nullptr;
    }
    if (apps.string_pool != nullptr)
    {
        delete[] apps.string_pool;
        apps.string_pool = nullptr;
    }
    if (apps.id_to_string != nullptr)
    {
        delete[] apps.id_to_string;
        apps.id_to_string = nullptr;
    }

    if (resources.nodes != nullptr)
    {
        delete[] resources.nodes;
        resources.nodes = nullptr;
    }
    if (resources.string_pool != nullptr)
    {
        delete[] resources.string_pool;
        resources.string_pool = nullptr;
    }
    if (resources.id_to_string != nullptr)
    {
        delete[] resources.id_to_string;
        resources.id_to_string = nullptr;
    }
}