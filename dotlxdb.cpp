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

using json = nlohmann::json;

class unableToParseChar: public std::exception {
    char ch;
    std::string msg;
public:
    explicit unableToParseChar(char c) : ch(c), msg("Unable to parse given character: " + std::string(1, c)) {}
    const char* what() const noexcept override {
        return msg.c_str();
    }
};

class DB {
public:
    struct ValuePair {
        std::string variable;
        std::string value;
    };

    std::vector<ValuePair> Columns;
    std::vector<std::map<int, std::vector<ValuePair>>> Table;

    DB() = default;

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

DB db;

int Index = 1;
int Indexes = 0;

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
    static int column_id = -1;
    std::string scolumn_id = "";

    int i = 0;

    while (line[i] != ' ')
    {
        scolumn_id += line[i];
        i++;
    }
    i++;
    column_id = std::stoi(scolumn_id);
    Indexes++;
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
    DB ExternDb;

    std::ifstream input{filepath};
    if (!input.is_open()) {
        return _strdup("{}");
    }

    std::vector<std::string> lines;
    for (std::string line; std::getline(input, line);) lines.push_back(std::move(line));
    for (auto& line : lines) lexer(line, ExternDb);

    std::string jsonStr = ExternDb.TableToJson().dump(2);
    return _strdup(jsonStr.c_str());
}

void FreeString(char* ptr)
{
    free(ptr);
}

DB InternalReadDB(std::string& filepath) {
    DB ExternDb;
    std::vector<std::string> file;
    std::ifstream input{filepath};
    if (!input.is_open()) {
        printf("Could not read file");
        return ExternDb;
    }
    for(std::string line; std::getline(input, line);){
        file.push_back(std::move(line));
    }
    for( std::string& line : file){
        lexer(line, ExternDb);
    }
    return ExternDb;
}

DB WriteDB(std::string filepath) {
    DB ExternDB;
    return ExternDB;
}

DB UpdateDB(std::string filepath) {
    DB ExternDB;
    return ExternDB;
}

int main(int argc, char* argv[]) {
    std::string filepath;
    if (argc < 2){
        printf("Did not specify file.\n");
        return 1;
    } else {
        filepath = argv[1];
    }

    db = InternalReadDB(filepath);
    printf("File parsed. Found %i indexes\n", Indexes);
    db.PrintTable();
    return 0;
}
