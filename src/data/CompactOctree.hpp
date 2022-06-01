#pragma once

#include "RawOctree.hpp"
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>

#include "../builder/DiskTree.hpp"

#include "stb_dxt.h"
#include "../util/DiskIO.hpp"

struct CompactOctree {
    struct Node {
        uint32_t leafs = 0;
        std::array<uint32_t, 8> children {};

        bool isLeaf() const {
            for (const auto &child: children)
                if (child != 0)
                    return false;

            return true;
        }
    };

    static_assert(sizeof(Node) == 36);

    std::vector<Node> dag;

    struct Colors : std::vector<uint8_t> {
        struct Header {
            bool compressed = false;

            uint32_t blocksX = 0;
            uint32_t blocksY = 0;
        } header;
    } colors;

    CompactOctree() = default;

    void saveToDisk(const std::string &path) {
        {
            auto output = DiskIO::openBinaryOutput(path + ".dag");
            DiskIO::writeVec(output, dag);
        }
        {
            auto output = DiskIO::openBinaryOutput(path + ".colors");
            DiskIO::write(output, colors.header);
            DiskIO::writeVec(output, colors);
        }
    }

    explicit CompactOctree(const std::string &path) {
        {
            auto input = DiskIO::openBinaryInput(path + ".dag");
            DiskIO::readToVec(input, dag);
        }
        {
            auto input = DiskIO::openBinaryInput(path + ".colors");
            DiskIO::read(input, colors.header);
            DiskIO::readToVec(input, colors);
        }
    }
};
