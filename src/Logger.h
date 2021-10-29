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
        const std::string& getRepr() const;

        template<typename T>
        Printable(T t) : repr(std::to_string(t)) {}

        Printable(glm::vec3 t);
        Printable(glm::uvec3 t);
        Printable(const char *cStr);
    };

    void info(const std::vector<Printable> &v);
};

inline Logger Log;
