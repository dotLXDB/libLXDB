#ifndef DOTLXDB_EXCEPTIONS_H
#define DOTLXDB_EXCEPTIONS_H

#include <exception>
#include <string>

class unableToAccessFile : public std::exception {
    std::string filepath;
    std::string msg;
public:
    explicit unableToAccessFile(const std::string& filepath)
        : filepath(filepath), msg("Unable to access file: " + filepath) {}

    [[nodiscard]] const char* what() const noexcept override {
        return msg.c_str();
    }
};

#endif //DOTLXDB_EXCEPTIONS_H