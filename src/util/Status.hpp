#pragma once

#include <string>

struct Status {
    enum Value {
        OK,
        ERROR
    } val;

    std::string message;

    Status() = delete;
    Status(Value, std::string);

    static Status ok();
    static Status error(const std::string &what);

    void assertOK();
};
