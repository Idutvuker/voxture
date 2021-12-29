#pragma once

#include <iostream>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/gtx/string_cast.hpp>

class Logger {
    std::ostream &outStream = std::cout;
public:
    class Printable {
        std::string repr;
    public:
        template<typename T,
                std::enable_if_t<std::is_fundamental_v<T>, bool> = true>
        Printable(const T &t) : repr(std::to_string(t)) {}

        template<typename T,
                std::enable_if_t<!std::is_fundamental_v<T>, bool> = true>
        Printable(const T &t) : repr(glm::to_string(t)) {}

        Printable(const std::string &str) : repr(str) {}

        Printable(const char *cStr) : repr(cStr) {}

        const std::string& getRepr() const {
            return repr;
        }
    };

    void info(const std::vector<Printable> &v) {
        for (const auto &p : v)
            outStream << p.getRepr() << ' ';
        outStream << std::endl;
    }
};

inline Logger Log;
