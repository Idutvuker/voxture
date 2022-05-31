#pragma once

#include "RawOctree.hpp"
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>

#include "../builder/DiskTree.hpp"

#include "stb_dxt.h"

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
    std::vector<uint8_t> colors;


    CompactOctree() = default;

    void saveToDisk(const std::string &path) {
        {
            std::ofstream output(path + ".dag", std::ios::out | std::ios::binary);
            output.write(reinterpret_cast<const char *>(dag.data()), sizeof(Node) * dag.size());
        }
        {
            std::ofstream output(path + ".colors", std::ios::out | std::ios::binary);
            output.write(reinterpret_cast<const char *>(colors.data()), sizeof(uint8_t) * colors.size());
        }
    }

    void compressColors() {
        using u8 = uint8_t;

        std::vector<u8> dst(10, 0);

        stb_compress_dxt_block(dst.data(), reinterpret_cast<const unsigned char *>(colors.data()), 0, STB_DXT_NORMAL);

        for (uint i = 0; i < 10; i++) {
            std::cout << i << ' ' << int(dst[i]) << std::endl;
        }
    }

    explicit CompactOctree(const std::string &path) {
        {
            std::ifstream input(path + ".dag", std::ios::in | std::ios::binary);

            Node node;
            while (input.read(reinterpret_cast<char *>(&node), sizeof(Node)))
                dag.push_back(node);
        }
        {
            std::ifstream input(path + ".colors", std::ios::in | std::ios::binary);

            uint8_t color;
            while (input.read(reinterpret_cast<char *>(&color), sizeof(uint8_t)))
                colors.push_back(color);
        }
    }
};
