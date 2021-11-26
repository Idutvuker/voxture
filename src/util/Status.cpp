#include "Status.hpp"

#include <iostream>
#include <utility>

void Status::assertOK() {
    if (val != OK) {
        std::cerr << "Assert statusOK failed. Message: " << message << std::endl;
        std::terminate();
    }
}

Status::Status(Status::Value val, std::string message) : val(val), message(std::move(message)) {}

Status Status::ok() {
    return {OK, {}};
}

Status Status::error(const std::string &what) {
    return {ERROR, what};
}
