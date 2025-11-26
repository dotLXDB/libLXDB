#pragma once
#include <map>
#include <string>
#include <vector>

#ifdef LXDB_EXPORT
#define API extern "C" __declspec(dllexport)
#else
#define API extern "C" __declspec(dllimport)
#endif

#ifndef DOTLXDB_LXDB_H
#define DOTLXDB_LXDB_H

#endif

class DB;

std::vector<std::string> splitString(const std::string &str, const char delimiter);

void lexer(std::string& line, DB& db);

API char* ReadDB(const char* filepath);

API bool UpdateDB(const char* filepath, int index, const char** keys, const char** values, int count);

API void FreeString(char* ptr);

DB InternalReadDB(std::string& filepath);

DB WriteDB(std::string filepath);

DB UpdateDB(std::string filepath);

int main(int argc, char* argv[]);
