#pragma once

#include "Octree.hpp"
#include <fstream>
#include <vector>
#include <queue>

struct CompactTree {
    using u32 = uint32_t;
    using u8 = uint8_t;

    struct Node {
        u32 meta = 0;
        u32 children = 0;

        static constexpr u32 childMask = 0xff000000;
        static constexpr u32 maskOffset = 24;

        u8 getChildMask() {
            return meta >> 24;
        }

        bool isLeaf() {
            return getChildMask() == 0;
        }
    };

    static_assert(sizeof(Node) == 8);

    std::vector<Node> data;

    explicit CompactTree(const Octree &octree) {
        std::queue<u32> nodeQueue;
        nodeQueue.push(0);

        data.emplace_back();
        size_t cur = 0;

        while (!nodeQueue.empty()) {
            u32 v = nodeQueue.front();
            nodeQueue.pop();

            const Octree::Node &oldNode = octree.data[v];
            data[cur].meta = (~Node::childMask) & oldNode.color;
            data[cur].children = data.size();

            for (u32 i = 0; i < oldNode.children.size(); i++) {
                u32 childOffs = oldNode.children[i];

                if (childOffs != 0) {
                    nodeQueue.push(v + childOffs);

                    data[cur].meta |= (1 << i << Node::maskOffset);
                    data.emplace_back();
                }
            }

            Node n = data[cur];

            cur += 1;
        }
    }

    void saveToDisk(const std::string &path) {
        std::ofstream output(path, std::ios::out | std::ios::binary);

        output.write(reinterpret_cast<const char *>(data.data()), sizeof(Node) * data.size());
    }

    explicit CompactTree(const std::string &path) {
        std::ifstream input(path, std::ios::in | std::ios::binary);

        Node node;
        while (input.read(reinterpret_cast<char *>(&node), sizeof(Node)))
            data.push_back(node);
    }
};