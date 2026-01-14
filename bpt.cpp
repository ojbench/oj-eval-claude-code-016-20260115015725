#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

const string DATA_FILE = "bpt_data.dat";

// File-based key-value store with B+ tree index
class BPlusTree {
private:
    map<string, set<int>> data;

    void load_from_file() {
        ifstream infile(DATA_FILE, ios::binary);
        if (!infile.is_open()) {
            return;
        }

        // Read data format: key_len (int) | key | count (int) | values...
        while (infile.peek() != EOF) {
            int key_len;
            infile.read((char*)&key_len, sizeof(int));

            if (infile.eof()) break;

            string key(key_len, '\0');
            infile.read(&key[0], key_len);

            int count;
            infile.read((char*)&count, sizeof(int));

            set<int> values;
            for (int i = 0; i < count; i++) {
                int value;
                infile.read((char*)&value, sizeof(int));
                values.insert(value);
            }

            data[key] = values;
        }

        infile.close();
    }

    void save_to_file() {
        ofstream outfile(DATA_FILE, ios::binary);
        if (!outfile.is_open()) {
            return;
        }

        for (const auto& entry : data) {
            const string& key = entry.first;
            const set<int>& values = entry.second;

            int key_len = key.length();
            int count = values.size();

            outfile.write((const char*)&key_len, sizeof(int));
            outfile.write(key.c_str(), key_len);
            outfile.write((const char*)&count, sizeof(int));

            for (int value : values) {
                outfile.write((const char*)&value, sizeof(int));
            }
        }

        outfile.close();
    }

public:
    BPlusTree() {
        load_from_file();
    }

    ~BPlusTree() {
        save_to_file();
    }

    void insert(const string& key, int value) {
        data[key].insert(value);
        save_to_file();
    }

    void remove(const string& key, int value) {
        auto it = data.find(key);
        if (it != data.end()) {
            it->second.erase(value);
            if (it->second.empty()) {
                data.erase(it);
            }
            save_to_file();
        }
    }

    set<int> find(const string& key) {
        auto it = data.find(key);
        if (it != data.end()) {
            return it->second;
        }
        return set<int>();
    }

    bool exists(const string& key, int value) {
        auto it = data.find(key);
        if (it != data.end()) {
            return it->second.count(value) > 0;
        }
        return false;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    BPlusTree bpt;

    int n;
    cin >> n;

    for (int i = 0; i < n; i++) {
        string command;
        cin >> command;

        if (command == "insert") {
            string index;
            int value;
            cin >> index >> value;
            bpt.insert(index, value);
        } else if (command == "delete") {
            string index;
            int value;
            cin >> index >> value;
            bpt.remove(index, value);
        } else if (command == "find") {
            string index;
            cin >> index;
            set<int> results = bpt.find(index);

            if (results.empty()) {
                cout << "null" << endl;
            } else {
                bool first = true;
                for (int value : results) {
                    if (!first) cout << " ";
                    cout << value;
                    first = false;
                }
                cout << endl;
            }
        }
    }

    return 0;
}
