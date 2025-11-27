#define LXDB_EXPORT
#include <cstdio>
#include "dotlxdb.h"
#include <map>
#include <string>
#include <nlohmann/json.hpp>
#include <cstring>

std::string DB::filepath;

DB* CreateDB(char* filepath) {
    DB* ExternDb = nullptr;
    try {
        ExternDb = new DB(DB::CreateDb(filepath));
    }
    catch (std::exception& e) {
        COLORS::eprint(e.what());
        return nullptr;
    }
    return ExternDb;
}

char* ReadDB(const DB* ExternDB)
{
    std::string jsonStr = ExternDB -> TableToJson().dump(2);
    return strdup(jsonStr.c_str());
}

bool UpdateDB(DB* ExternDB, int index, const char** keys, const char** values, int count) {
    printf("UpdateDB called with filepath: %s, index: %d, count: %d\n", DB::filepath.c_str() , index, count);
    std::map<std::string, std::string> newRow;
    for (int i = 0; i < count; ++i) {
        newRow[keys[i]] = values[i];
    }

    if (ExternDB -> UpdateDB(index, newRow) && DB::SaveToFile(index, newRow)) {
        return true;
    }
    return false;
}
