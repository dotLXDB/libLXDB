#define LXDB_EXPORT
#include <cstdio>
#include "dotlxdb.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <bits/stdc++.h>
#include <nlohmann/json.hpp>
#include <print>
#include "exceptions.h"

using json = nlohmann::json;

class COLORS {
    public:
        static void eprint(std::string str) {
            std::cerr << "\033[0;31m" << str << "\033[0m" << std::endl;
        }
};

class DB {
public:
    struct ValuePair {
        std::string variable;
        std::string value;
    };

    typedef std::map<std::string, std::string> Row;

    std::vector<ValuePair> Columns;
    std::vector<std::map<int, std::vector<ValuePair>>> Table;

    DB() = default;

    DB CreateDb(const char* filepath)
    {
        DB db;
        std::ifstream input;

        try {
            input.open(filepath);
            if (!input.is_open()) {
                throw unableToAccessFile(filepath);
            }
            std::vector<std::string> lines;
            for (std::string line; std::getline(input, line);) {
                lines.push_back(std::move(line));
            }
            for (auto& line : lines) {
                lexer(line, db);
            }
        }
        catch (const std::exception& e) {
            COLORS::eprint(e.what());
        }
        return db;
    }

    bool UpdateDB(int index, DB::Row& row) {
        if (index >= Table.size()) return false;
        Table[index].clear();
        std::vector<ValuePair> newVector;
        for (const auto& [k, v] : row) {
            newVector.push_back({k, v});
        }
        Table[index].insert({ index, newVector});
        return true;
    }


    bool SaveToFile(const char* filepath, int rowId, const DB::Row& newRow) {
        try {
            std::ifstream input(filepath);
            if (!input.is_open()){
            throw unableToAccessFile(filepath);
            };
            std::vector<std::string> lines;
            for (std::string line; std::getline(input, line);)
                lines.push_back(line);
            input.close();
            if (rowId <= 0 || rowId > (int)lines.size()) return false;
            std::string newLine = std::to_string(rowId) + " => { ";
            size_t i = 0;
            for (const auto& [k, v] : newRow) {
                newLine += k + " \"" + v + "\"";
                if (i + 1 < newRow.size()) newLine += "; ";
                i++;
            }
            newLine += " }";
            lines[rowId - 1] = newLine;
            std::ofstream output(filepath, std::ios::trunc);
            if (!output.is_open()) return false;
            for (const auto& l : lines) output << l << "\n";
            return true;
        }catch (const std::exception& e) {
            COLORS::eprint(e.what());
        }
    }

    std::vector<ValuePair>* GetRowById(int id) {
        for (auto& rowMap : Table) {
            auto it = rowMap.find(id);
            if (it != rowMap.end()) {
                return &(it->second);
            }
        }
        return nullptr;
    }

    void AddData(int index, ValuePair& column) {
        if (index >= Table.size()) {
            Table.resize(index + 1);
        }
        Table[index][index].push_back(column);
    }
    void PrintTable() {
        for (size_t idx = 0; idx < Table.size(); idx++) {
            const auto& rowMap = Table[idx];

            for (const auto& [key, vec] : rowMap) {
                std::cout << "ID: " << idx << " Key:\n";
                for (const auto& vp : vec) {
                    std::cout << "  " << vp.variable << " = " << vp.value << "\n";
                }
            }
        }
    }
    json TableToJson() {
        json table = json::array();
        for (const auto& rowMap : Table) {
            json rowJson = json::object();
            for (const auto& [key, vec] : rowMap) {
                json obj = json::object();
                for (const auto& vp : vec) {
                    obj[vp.variable] = vp.value;
                }
                rowJson[std::to_string(key)] = obj;
            }
            table.push_back(rowJson);
        }
        return table;
    }
};

std::vector<std::string> splitString(const std::string &str, const char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        if (!item.empty()) {
            tokens.push_back(item);
        }
    }
    return tokens;
}

void lexer(std::string& line, DB& db) {
    int column_id = -1;
    std::string scolumn_id = "";

    int i = 0;

    while (line[i] != ' ')
    {
        scolumn_id += line[i];
        i++;
    }
    i++;
    column_id = std::stoi(scolumn_id);
    if(line[i] == '=' && line[i+1] == '>')
        i = i+5;

    std::string& pairs = line;

    std::string cleaned = pairs.substr(i);
    size_t pos = cleaned.find_last_of('}');
    if (pos != std::string::npos) {
        cleaned = cleaned.substr(0, pos);
    } else {
        cleaned.clear();
    }

    std::vector<std::string> ValuePairs = splitString(cleaned, ';');

    for (std::string& part : ValuePairs) {

        while (!part.empty() && std::isspace(part.front())) part.erase(part.begin());
        while (!part.empty() && std::isspace(part.back())) part.pop_back();

        if (part.empty())
            continue;

        int j = 0;

        std::string column_name;
        std::string column_value;

        while (j < part.size() && part[j] != ' ') {
            column_name += part[j];
            j++;
        }

        if (j < part.size()) j++;

        if (j < part.size() && part[j] == '"') {
            j++;
            while (j < part.size() && part[j] != '"') {
                column_value += part[j];
                j++;
            }
        }

        DB::ValuePair vp { column_name, column_value };
        db.AddData(column_id, vp);
    }
}

char* ReadDB(const char* filepath)
{
    DB ExternDb = DB().CreateDb(filepath);

    std::string jsonStr = ExternDb.TableToJson().dump(2);
    return strdup(jsonStr.c_str());
}

bool UpdateDB(const char* filepath, int index, const char** keys, const char** values, int count) {
    printf("UpdateDB called with filepath: %s, index: %d, count: %d\n", filepath, index, count);
    std::map<std::string, std::string> newRow;
    for (int i = 0; i < count; ++i) {
        newRow[keys[i]] = values[i];
    }

    DB ExternDB = DB().CreateDb(filepath);

    if (ExternDB.UpdateDB(index, newRow) && ExternDB.SaveToFile(filepath, index, newRow)) {
        return true;
    }
    return false;
}


void FreeString(char* ptr)
{
    free(ptr);
}


//Used only for testing internally before using in C# wrapper tests

int main(int argc, char* argv[]) {
    std::string filepath;
    if (argc < 2){
        printf("Did not specify file.\n");
        return 1;
    } else {
        filepath = argv[1];
    }

    DB db = DB().CreateDb(filepath.c_str());
    DB::Row x = {
        {"name", "Lil"},
        { "last_name", "Uzi"}
    };
    if (db.UpdateDB(3, x) && db.SaveToFile(filepath.c_str(), 3, x)) {
        printf("Successfully updated.\n");
    }
    else
        printf("Failed to update DB.\n");
    printf("File parsed.\n");
    db.PrintTable();
    return 0;
}
