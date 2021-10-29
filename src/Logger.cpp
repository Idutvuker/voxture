#include "Logger.h"

#include <utility>

const std::string &Logger::Printable::getRepr() const {
    return repr;
}

Logger::Printable::Printable(glm::vec3 t) : repr(glm::to_string(t)) {}
Logger::Printable::Printable(glm::uvec3 t) : repr(glm::to_string(t)) {}

Logger::Printable::Printable(const char *cStr) : repr(cStr) {}

void Logger::info(const std::vector<Printable> &v) {
    for (const auto &p : v)
        outStream << p.getRepr() << ' ';
    outStream << std::endl;
}
