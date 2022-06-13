#pragma once

#include <fstream>
#include <string>

struct Config {
    std::filesystem::path bundlePath;
    std::filesystem::path builderOutPath;
    std::filesystem::path viewerTexturePath;

    Config(const std::string &path) {
        std::ifstream input(path);

        std::string token;
        while (input >> token) {
            if (token == "bundleDir:") {
                input >> token;
                bundlePath = token;
            }

            else if (token == "builderOutputDir:") {
                input >> token;
                builderOutPath = token;

                if (!exists(builderOutPath)) {
                    create_directory(builderOutPath);
                }
            }

            else if (token == "viewerTexture:") {
                input >> token;
                viewerTexturePath = token;
            }
        }

        assert(!bundlePath.empty());
        assert(!builderOutPath.empty());
        assert(!viewerTexturePath.empty());

        assert(is_directory(bundlePath));
        assert(is_directory(builderOutPath));
    }
};