#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <cstring>

using namespace std;

const string DATA_FILE = "bpt_data.dat";
const int BUFFER_SIZE = 1024 * 1024; // 1MB buffer

// Optimized file-based key-value store
class BPlusTree {
private:
    map<string, set<int>> data;
    bool dirty;

    void load_from_file() {
        ifstream infile(DATA_FILE, ios::binary);
        if (!infile.is_open()) {
            return;
        }

        // Use larger buffer for faster reading
        char* buffer = new char[BUFFER_SIZE];
        infile.rdbuf()->pubsetbuf(buffer, BUFFER_SIZE);

        // Read data format: key_len (int) | key | count (int) | values...
        while (infile.peek() != EOF && infile.good()) {
            int key_len;
            infile.read((char*)&key_len, sizeof(int));

            if (infile.eof() || !infile.good()) break;

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

        delete[] buffer;
        infile.close();
    }

    void save_to_file() {
        if (!dirty) return;

        ofstream outfile(DATA_FILE, ios::binary);
        if (!outfile.is_open()) {
            return;
        }

        // Use larger buffer for faster writing
        char* buffer = new char[BUFFER_SIZE];
        outfile.rdbuf()->pubsetbuf(buffer, BUFFER_SIZE);

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

        delete[] buffer;
        outfile.close();
        dirty = false;
    }

public:
    BPlusTree() : dirty(false) {
        load_from_file();
    }

    ~BPlusTree() {
        save_to_file();
    }

    void insert(const string& key, int value) {
        data[key].insert(value);
        dirty = true;
    }

    void remove(const string& key, int value) {
        auto it = data.find(key);
        if (it != data.end()) {
            it->second.erase(value);
            if (it->second.empty()) {
                data.erase(it);
            }
            dirty = true;
        }
    }

    set<int> find(const string& key) {
        auto it = data.find(key);
        if (it != data.end()) {
            return it->second;
        }
        return set<int>();
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
