#pragma once

#include "RawOctree.hpp"
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>

#include "../builder/DiskTree.hpp"

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

    using Color = uint32_t;

    std::vector<Node> dag;
    std::vector<Color> colors;


    CompactOctree() = default;

    void saveToDisk(const std::string &path) {
        {
            std::ofstream output(path + ".dag", std::ios::out | std::ios::binary);
            output.write(reinterpret_cast<const char *>(dag.data()), sizeof(Node) * dag.size());
        }
        {
            std::ofstream output(path + ".colors", std::ios::out | std::ios::binary);
            output.write(reinterpret_cast<const char *>(colors.data()), sizeof(Color) * colors.size());
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

            Color color;
            while (input.read(reinterpret_cast<char *>(&color), sizeof(Color)))
                colors.push_back(color);
        }
    }
};
