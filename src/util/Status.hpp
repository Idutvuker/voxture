#pragma once

#include <string>
#include <iostream>

struct Status {
    enum Value {
        OK,
        ERROR
    } val;

    std::string message;

    Status() = delete;
    Status(Status::Value val, std::string message) : val(val), message(std::move(message)) {}

    void assertOK() {
        if (val != OK) {
            std::cerr << "Assert statusOK failed. Message: " << message << std::endl;
            std::terminate();
        }
    }

    static Status ok() {
        return {OK, {}};
    }

    static Status error(const std::string &what) {
        return {ERROR, what};
    }

};
