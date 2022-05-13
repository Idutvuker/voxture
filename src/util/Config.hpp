#pragma once

#include <fstream>
#include <string>

struct Config {
    std::string builderBundlePath;
    std::string builderOutPath;
    std::string viewerBundlePath;
    std::string viewerTexturePath;
    std::string options;

    Config(const std::string &path) {
        std::ifstream input(path);
        input >> builderBundlePath >> builderOutPath;
        input >> viewerBundlePath >> viewerTexturePath;
        input >> options;
    }
};