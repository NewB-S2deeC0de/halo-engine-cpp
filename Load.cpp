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
        id_to_string = new string_view[capacity];
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
        if (idx == start_idx)
        {
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

    id_to_string[current_id] = nodes[idx].key;

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
        if (idx == start_idx)
        {
            cout << "bang app da day\n";
            break;
        }
    }

    char *str = string_pool + pool_offset;
    memcpy(str, sv.data(), sv.size());
    pool_offset += sv.size();

    nodes[idx].key = string_view(str, sv.size());
    nodes[idx].id = current_id;

    id_to_string[current_id] = nodes[idx].key;

    current_id++;
    return nodes[idx].id;
}

int StringDict::find(string_view sv) const
{
    int idx = hashing(sv, capacity);
    int start_idx = idx;

    while (nodes[idx].id != -1)
    {
        if (nodes[idx].key == sv)
        {
            return idx;
        }
        idx = (idx + 1) % capacity;
        if (idx == start_idx)
        {
            break;
        }
    }
    return -1;
}

string_view nextToken(const char *&p, const char *end)
{
    while (p < end && (*p == '\r' || *p == '\n'))
    {
        p++;
    }
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

int8_t lookupLocation(string_view sv)
{
    if (sv.size() != 2)
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
        if(sv == "LOGIN") return 0;        
        break;
    }
    case 6:
    {
        if (sv == "LOGOUT") return 1; 
        if (sv == "ACCESS") return 3;
        break;
    }
    case 8:
    {
        if (sv == "OPEN_APP") return 5;
        if (sv == "DOWNLOAD") return 6;
        break;
    }
    case 12:
    {
        if (sv == "FAILED_LOGIN") return 4;
        if (sv == "ADMIN_ACTION") return 7;
        break;
    }
    case 13:
    {
        if (sv == "TOKEN_REFRESH") return 2;
        break;
    }
    }
    return -1;
}

inline bool isValidID(string_view sv, string_view prefix) 
{
    // Kiem tra chuoi co dai hon tham so prefix hay khogn
    if (sv.size() <= prefix.size())
    {
        return false;
    }

    // Kiem tra tien to thuc te co khop voi tien to ly thuyet (tham so prefix) hay khong
    if (sv.substr(0, prefix.size()) != prefix) 
    {
        return false;
    }

    // Kiem tra phan con lai co hoan toan la so ('0' -> '9') hay khong
    for (size_t i = prefix.size(); i < sv.size(); i++) 
    {
        if (sv[i] < '0' || sv[i] > '9')
        {
            return false;
        }
    }

    return true;
}

void loadData(const char *filename, Log *&logs, StringDict &users, StringDict &devices, StringDict &apps, StringDict &resources, char *&buf, int &count, long long ts_now)
{
    FILE *f = fopen(filename, "rb");
    if (f == nullptr)
    {
        return;
    }

    FILE *error_f = fopen("error_logs.csv", "wb");

    int character;
    while ((character = fgetc(f)) != EOF && character != '\n')
    {
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

            const char* start_of_line = p;

            string_view user_sv = nextToken(p, end);
            string_view device_sv = nextToken(p, end);
            string_view app_sv = nextToken(p, end);
            string_view resource_sv = nextToken(p, end);
            string_view event = nextToken(p, end);
            string_view location = nextToken(p, end);
            string_view timestamp = nextToken(p, end);

            // Kiem tra token rong
            if (user_sv.empty() || device_sv.empty() || app_sv.empty() || resource_sv.empty() || 
                event.empty() || location.empty() || timestamp.empty())
            {
                if (error_f)
                {
                    fwrite(start_of_line, 1, p - start_of_line, error_f);
                }
                continue;
            }
            int8_t event_type_idx = lookupEvent(event);
            int8_t location_idx = lookupLocation(location); 
            // Kiem tra token sai tien to, hau to khong
            if (!isValidID(user_sv, "U") ||
                !isValidID(device_sv, "D") ||
                !isValidID(app_sv, "APP") ||
                !isValidID(resource_sv, "R") ||
                event_type_idx == -1 || 
                location_idx == -1)
            {
                if (error_f) {
                    fwrite(start_of_line, 1, p - start_of_line, error_f);
                }
                continue;
            }

            // Kiem tra ts co chua toan ky tu so hay khong
            bool is_valid_ts = true;
            for(char c : timestamp) 
            {
                if (c < '0' || c > '9')
                {
                    is_valid_ts = false;
                    break;
                }
            }

            if (!is_valid_ts) 
            {
                if (error_f) 
                {
                    fwrite(start_of_line, 1, p - start_of_line, error_f);
                }
                continue;
            }

            // Kiem tra ts co hop le khong
            long long ts = 0;
            std::from_chars(timestamp.data(), timestamp.data() + timestamp.size(), ts);
            if (ts < 1600000000 || ts > ts_now)
            {
                if (error_f) 
                {
                    fwrite(start_of_line, 1, p - start_of_line, error_f);
                }
                continue;
            }

            logs[count].user_id = users.getOrAdd(user_sv, count, logs[count].next_user_log, CAPACITY);
            logs[count].resource_id = resources.getOrAdd(resource_sv, count, logs[count].next_resource_log, CAPACITY);

            logs[count].device_id = devices.getOrAdd(device_sv, count, logs[count].next_device_log, CAPACITY);
            logs[count].app_id = apps.getOrAddSimple(app_sv, APP_CAPACITY);

            logs[count].event_type_index = event_type_idx;
            logs[count].location_index = location_idx;

            logs[count].timestamp = ts;         

            count++;
        }

        leftover_bytes = end - safe_end;
        if (leftover_bytes > 0)
        {
            memmove(buf, safe_end, leftover_bytes);
        }
    }

    if (error_f) 
    {
        fclose(error_f);
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

void swap(int32_t &a, int32_t &b)
{
    int32_t temp = a; 
    a = b; 
    b = temp; 
}

int lomutoPartition(int32_t *indices, int left, int right, const Log *logs)
{

    long long pivot = logs[indices[right]].timestamp;
    int i = left - 1;

    for (int j = left; j < right; j++)
    {
        if (logs[indices[j]].timestamp <= pivot)
        {
            i++;
            swap(indices[i], indices[j]);
        }
    }

    i++;
    swap(indices[i], indices[right]);
    return i;
}

void quickSort(int32_t *indices, int left, int right, const Log *logs)
{
    if (left >= right)
    {
        return;
    }
    int p = lomutoPartition(indices, left, right, logs);
    quickSort(indices, left, p - 1, logs);
    quickSort(indices, p + 1, right, logs);
}

void rebuildUserChains(StringDict &dict, Log *logs, int line_count)
{
    int32_t *temp_indices = new int32_t[line_count];

    for (int i = 0; i < dict.capacity; i++)
    {
        int32_t curr_index = dict.nodes[i].head_log_index;

        if (curr_index == -1)
        {
            continue;
        }

        int count = 0;
        while (curr_index != -1)
        {
            temp_indices[count] = curr_index;
            count++;

            curr_index = logs[curr_index].next_user_log;
        }

        if (count <= 1)
        {
            continue;
        }

        quickSort(temp_indices, 0, count - 1, logs);

        dict.nodes[i].head_log_index = temp_indices[0];

        for (int k = 0; k < count - 1; k++)
        {
            logs[temp_indices[k]].next_user_log = temp_indices[k + 1];
        }

        logs[temp_indices[count - 1]].next_user_log = -1;
    }

    delete[] temp_indices;
}

void rebuildResourceChains(StringDict &dict, Log *logs, int line_count)
{
    int32_t *temp_indices = new int32_t[line_count];

    for (int i = 0; i < dict.capacity; i++)
    {
        int32_t curr_index = dict.nodes[i].head_log_index;

        if (curr_index == -1)
        {
            continue;
        }

        int count = 0;
        while (curr_index != -1)
        {
            temp_indices[count] = curr_index;
            count++;

            curr_index = logs[curr_index].next_resource_log;
        }

        if (count <= 1)
        {
            continue;
        }

        quickSort(temp_indices, 0, count - 1, logs);

        dict.nodes[i].head_log_index = temp_indices[0];

        for (int k = 0; k < count - 1; k++)
        {
            logs[temp_indices[k]].next_resource_log = temp_indices[k + 1];
        }

        logs[temp_indices[count - 1]].next_resource_log = -1;
    }

    delete[] temp_indices;
}

void rebuildDeviceChains(StringDict &dict, Log* logs, int line_count) {
    int32_t *temp_indices = new int32_t[line_count];

    for (int i = 0; i < dict.capacity; i++)
    {
        int32_t curr_index = dict.nodes[i].head_log_index;

        if (curr_index == -1)
        {
            continue;
        }

        int count = 0;
        while (curr_index != -1)
        {
            temp_indices[count] = curr_index;
            count++;

            curr_index = logs[curr_index].next_device_log;
        }

        if (count <= 1)
        {
            continue;
        }

        quickSort(temp_indices, 0, count - 1, logs);

        dict.nodes[i].head_log_index = temp_indices[0];

        for (int k = 0; k < count - 1; k++)
        {
            logs[temp_indices[k]].next_device_log = temp_indices[k + 1];
        }

        logs[temp_indices[count - 1]].next_device_log = -1;
    }

    delete[] temp_indices;
}

void queryByUserID(string_view id, long long t1, long long t2,
                   const StringDict &users, const StringDict &devices,
                   const StringDict &apps, const StringDict &resources,
                   const Log *logs)
{
    int dict_id = users.find(id);
    if (dict_id == -1)
    {
        cout << "Khong tim thay: " << id << "\n";
        return;
    }

    int32_t current_log = users.nodes[dict_id].head_log_index;
    int match_count = 0;

    while (current_log != -1)
    {
        long long ts = logs[current_log].timestamp;

        if (ts > t2)
        {
            break;
        }
        if (ts >= t1)
        {
            string_view device_str = devices.id_to_string[logs[current_log].device_id];
            string_view app_str = apps.id_to_string[logs[current_log].app_id];
            string_view resource_str = resources.id_to_string[logs[current_log].resource_id];

            cout << device_str << " - " << app_str << " - " << resource_str << "\n";
            match_count++;
        }
        current_log = logs[current_log].next_user_log;
    }

    if (match_count == 0)
    {
        cout << "Nguoi dung nay khong co hoat dong nao trong thoi gian nay\n";
    }
}

void queryByResourceID(string_view id, long long t1, long long t2,
                       const StringDict &users, const StringDict &devices,
                       const StringDict &apps, const StringDict &resources,
                       const Log *logs)
{
    int dict_id = resources.find(id);
    if (dict_id == -1)
    {
        cout << "Khong tim thay: " << id << "\n";
        return;
    }

    int32_t current_log = resources.nodes[dict_id].head_log_index;
    int match_count = 0;

    while (current_log != -1)
    {
        long long ts = logs[current_log].timestamp;

        if (ts > t2)
        {
            break;
        }
        if (ts >= t1)
        {
            string_view user_sv = users.id_to_string[logs[current_log].user_id];
            string_view device_str = devices.id_to_string[logs[current_log].device_id];
            string_view app_str = apps.id_to_string[logs[current_log].app_id];

            cout << user_sv << " - " << device_str << " - " << app_str << "\n";
            match_count++;
        }
        current_log = logs[current_log].next_resource_log;
    }

    if (match_count == 0)
    {
        cout << "Nguoi dung nay khong co hoat dong nao trong thoi gian nay\n";
    }
}

void queryTop10Resources(long long t1, long long t2,
                         const StringDict &resources, Log *logs)
{
    TopResource top10[10];
    for (int i = 0; i < 10; i++)
    {
        top10[i].id = -1;
        top10[i].count = 0;
        top10[i].max_ts_log = -1;
    }

    for (int i = 0; i < resources.capacity; i++)
    {
        if (resources.nodes[i].id == -1)
        {
            continue;
        }

        int current_count = 0;
        int32_t current_max_ts_log = -1;
        int32_t current_log = resources.nodes[i].head_log_index;
        while (current_log != -1)
        {
            long long ts = logs[current_log].timestamp;

            if (ts > t2)
            {
                break;
            }

            if (ts >= t1)
            {
                current_count++;
                current_max_ts_log = current_log;
            }
            current_log = logs[current_log].next_resource_log;
        }

        if (current_count == 0)
        {
            continue;
        }

        bool replace = false;
        if (current_count > top10[9].count)
        {
            replace = true;
        }
        else if (current_count == top10[9].count)
        {
            long long ts1 = logs[current_max_ts_log].timestamp;
            long long ts2 = logs[top10[9].max_ts_log].timestamp;

            if (ts1 > ts2)
            {
                replace = true;
            }
        }

        if (replace)
        {
            top10[9].id = resources.nodes[i].id;
            top10[9].count = current_count;
            top10[9].max_ts_log = current_max_ts_log;

            for (int j = 8; j >= 0; j--)
            {
                bool swap = false;

                if (top10[j].id == -1)
                {
                    swap = true;
                }
                else if (top10[j + 1].count > top10[j].count)
                {
                    swap = true;
                }
                else if (top10[j + 1].count == top10[j].count)
                {
                    if (logs[top10[j + 1].max_ts_log].timestamp >= logs[top10[j].max_ts_log].timestamp)
                    {
                        swap = true;
                    }
                }

                if (swap)
                {
                    TopResource temp = top10[j];
                    top10[j] = top10[j + 1];
                    top10[j + 1] = temp;
                }
                else
                {
                    break;
                }
            }
        }
    }

    cout << "=== TOP 10 TAI NGUYEN DUOC TRUY CAP NHIEU NHAT ===\n";
    bool check = false;
    for (int i = 0; i < 10; i++)
    {
        if (top10[i].count > 0)
        {
            check = true;
            string_view res = resources.id_to_string[top10[i].id];
            cout << "Top " << (i + 1) << ": " << res << "| So luot: " << top10[i].count << "\n";
        }
    }

    if (!check)
    {
        cout << "Khong co truy cap nao den tai nguyen trong thoi gian nay\n";
    }
}

void detectConsecutiveFailedLogins(const StringDict &users, const Log *logs, const char* out_filename)
{
    FILE *f_out = fopen(out_filename, "w");
    if (f_out == nullptr)
    {
        cout << "Khong the tao file bao cao " << out_filename << "/n";
        return;
    }

    fprintf(f_out, "user_id,violation_type,failed_count\n");

    int total_short_violations = 0;
    int total_long_violations = 0;

    // Cac hang so cho cua so truot
    const int SHORT_LIMIT = 5;
    const long long SHORT_TIME = 60;

    const int LONG_LIMIT = 10;
    const long long LONG_TIME = 3 * 24 * 60 * 60;

    for (int i = 0; i < users.capacity; i++) 
    {
        if (users.nodes[i].id == -1)
        {
            continue;
        }

        int32_t curr_idx = users.nodes[i].head_log_index;

        long long short_window[SHORT_LIMIT];
        int short_count = 0;

        long long long_window[LONG_LIMIT];
        int long_count = 0;

        bool short_violated = false;
        bool long_violated = false;

        string_view user_sv = users.nodes[i].key;

        while (curr_idx != -1) 
        {
            if (logs[curr_idx].event_type_index == 4)   // FAILED_LOGIN
            {
                long long ts = logs[curr_idx].timestamp;

                if (!short_violated)
                {
                    short_window[short_count] = ts;
                    short_count++;

                    if (short_count == SHORT_LIMIT)
                    {
                        if (short_window[SHORT_LIMIT - 1] - short_window[0] <= SHORT_TIME)
                        {
                            short_violated = true;
                            fprintf(f_out, "%.*s,SHORT_BURST,%lld\n", 
                                    (int)user_sv.size(), user_sv.data(), ts);
                            total_short_violations++;
                        }
                        else
                        {
                            // Xoa phan tu dau tien
                            for (int k = 0; k < SHORT_LIMIT - 1; k++) 
                            {
                                short_window[k] = short_window[k + 1];
                            }
                            short_count--;
                        }
                    }
                }

                if (!long_violated)
                {
                    long_window[long_count] = ts;
                    long_count++;
                    if (long_count == LONG_LIMIT)
                    {
                        if (long_window[LONG_LIMIT - 1] - long_window[0] <= LONG_TIME)
                        {
                            long_violated = true;
                            fprintf(f_out, "%.*s,LONG_SLOW,%lld\n", 
                                    (int)user_sv.size(), user_sv.data(), ts);
                            total_long_violations++;
                        }
                        else
                        {
                            // Dich cua so len 1 log
                            for (int k = 0; k < LONG_LIMIT - 1; k++) {
                                long_window[k] = long_window[k + 1];
                            }
                            long_count--;
                        }
                    }
                }

                if (short_violated && long_violated)
                {
                    break;
                }
            }

            else if (logs[curr_idx].event_type_index == 0)
            {
                short_count = 0;
                long_count = 0;
            }

            curr_idx = logs[curr_idx].next_user_log;
        }
    }

    fclose(f_out);
    cout << "[Bao cao] Phat hien " << total_short_violations << " vi pham NGAN (Brute-force).\n";
    cout << "[Bao cao] Phat hien " << total_long_violations << " vi pham DAI (Low-and-Slow).\n";
    cout << "[Bao cao] Chi tiet duoc luu tai file: " << out_filename << "\n";
}

void detectMultipleDevicesLogin(const StringDict &users, const Log *logs, const char* out_filename)
{
    FILE *f_out = fopen(out_filename, "w");
    if (f_out == nullptr)
    {
        cout << "Loi: Khong the tao file bao cao " << out_filename << "\n";
        return;
    }

    fprintf(f_out, "user_id,unique_devices_count,window_end_timestamp\n");
    int total_violations = 0;

    const int DEVICE_THRESHOLD = 3;             // >= 3 thiet bi khac nhau
    const long long TIME_WINDOW = 10 * 60;      // 600s

    for (int i = 0; i < users.capacity; i++)
    {
        if (users.nodes[i].id == -1) 
        {
            continue;
        }

        int32_t curr_idx = users.nodes[i].head_log_index;
        bool violated = false;
        string_view user_sv = users.nodes[i].key;

        int32_t window_devices[100];
        long long window_timestamps[100];
        int w_size = 0;

        while (curr_idx != -1 && !violated)
        {
            if (logs[curr_idx].event_type_index == 0) // LOGIN
            {
                long long current_ts = logs[curr_idx].timestamp;

                if (w_size < 100)
                {
                    window_devices[w_size] = logs[curr_idx].device_id;
                    window_timestamps[w_size] = current_ts;
                    w_size++;
                }

                int expired_count = 0;
                while (expired_count < w_size && (current_ts - window_timestamps[expired_count]) > TIME_WINDOW)
                {
                    expired_count++;
                }
                
                if (expired_count > 0)
                {
                    for (int k = expired_count; k < w_size; k++)
                    {
                        window_devices[k - expired_count] = window_devices[k];
                        window_timestamps[k - expired_count] = window_timestamps[k];
                    }
                    w_size -= expired_count;
                }

                int unique_count = 0;
                for (int j = 0; j < w_size; j++)
                {
                    bool is_duplicate = false;
                    for (int k = 0; k < j; k++)
                    {
                        if (window_devices[j] == window_devices[k])
                        {
                            is_duplicate = true;
                            break;
                        }
                    }
                    if (!is_duplicate) unique_count++;
                }

                if (unique_count >= DEVICE_THRESHOLD)
                {
                    fprintf(f_out, "%.*s,%d,%lld\n", 
                           (int)user_sv.size(), user_sv.data(), 
                           unique_count, current_ts);
                    total_violations++;
                    violated = true; 
                }
            }
            curr_idx = logs[curr_idx].next_user_log;
        }
    }

    fclose(f_out);
    cout << "[Bao cao] Phat hien " << total_violations << " nguoi dung dang nhap nhieu thiet bi bat thuong.\n";
    cout << "[Bao cao] Chi tiet duoc luu tai file: " << out_filename << "\n";
}