#pragma once
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include "exceptions.h"

using json = nlohmann::json;

#ifdef LXDB_EXPORT
#define API extern "C"
#else
#define API extern "C"
#endif

#ifndef DOTLXDB_LXDB_H
#define DOTLXDB_LXDB_H

#endif

class COLORS {
    public:
        static void eprint(const std::string &str) {
            std::cerr << "\033[0;31m" << str << "\033[0m" << std::endl;
        }
};

API class DB {
public:
    struct ValuePair {
        std::string variable;
        std::string value;
    };

    static std::string filepath;

    typedef std::map<std::string, std::string> Row;

    std::vector<ValuePair> Columns;
    std::vector<std::map<int, std::vector<ValuePair>>> Table;

    DB() = default;

    static DB CreateDb(char* file)
    {
        DB db;

        try {
            std::ifstream input;
            filepath = file;
            input.open(filepath);
            if (!input.is_open()) {
                throw unableToAccessFile(filepath);
            }
            std::vector<std::string> lines;
            for (std::string line; std::getline(input, line);) {
                lines.push_back(std::move(line));
            }
            for (auto& line : lines) {
                parse(line, db);
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


    static bool SaveToFile(int rowId, const DB::Row& newRow) {
        try {
            std::ifstream input(filepath);
            if (!input.is_open()){
            throw unableToAccessFile(filepath);
            };
            std::vector<std::string> lines;
            for (std::string line; std::getline(input, line);)
                lines.push_back(line);
            input.close();
            if (rowId <= 0 || rowId > static_cast<int>(lines.size())) return false;
            std::string newLine = std::to_string(rowId) + " => { ";
            size_t i = 0;
            for (const auto& [k, v] : newRow) {
                newLine += k + " \"" += v + "\"";
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
        return false;
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

    void AddData(int index, const ValuePair& column) {
        if (index >= Table.size()) {
            Table.resize(index + 1);
        }
        Table[index][index].push_back(column);
    }
    void PrintTable() const {
        for (size_t idx = 0; idx < Table.size(); idx++) {
            const auto& rowMap = Table[idx];

            for (const auto &vec: rowMap | std::views::values) {
                std::cout << "ID: " << idx << " Key:\n";
                for (const auto& vp : vec) {
                    std::cout << "  " << vp.variable << " = " << vp.value << "\n";
                }
            }
        }
    }
    [[nodiscard]] json TableToJson() const {
        json table = json::array();
        for (const auto& rowMap : Table) {
            json rowJson = json::object();
            for (const auto& [key, vec] : rowMap) {
                json obj = nlohmann::json::object();
                for (const auto& vp : vec) {
                    obj[vp.variable] = vp.value;
                }
                rowJson[std::to_string(key)] = obj;
            }
            table.push_back(rowJson);
        }
        return table;
    }
    static void FreeString(char* ptr)
    {
        free(ptr);
    }
    private:
        static std::vector<std::string> splitString(const std::string &str) {
            std::vector<std::string> tokens;
            std::stringstream ss(str);
            std::string item;

            while (std::getline(ss, item, ';')) {
                if (!item.empty()) {
                    tokens.push_back(item);
                }
            }
            return tokens;
        }
        static void parse(const std::string& line, DB& db) {
            std::string scolumn_id;

            int i = 0;

            while (line[i] != ' ')
            {
                scolumn_id += line[i];
                i++;
            }
            i++;
            int column_id = std::stoi(scolumn_id);
            if(line[i] == '=' && line[i+1] == '>')
                i = i+5;

            const std::string& pairs = line;

            std::string cleaned = pairs.substr(i);
            size_t pos = cleaned.find_last_of('}');
            if (pos != std::string::npos) {
                cleaned = cleaned.substr(0, pos);
            } else {
                cleaned.clear();
            }

            std::vector<std::string> ValuePairs = splitString(cleaned);

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
};

API DB* CreateDB(char* filepath);

API char* ReadDB(const DB* ExternDB);

API bool UpdateDB(DB* ExternDB, int index, const char** keys, const char** values, int count);
