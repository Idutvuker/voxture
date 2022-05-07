#pragma once

#include "RawOctree.hpp"
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>

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

        bool operator==(const Node &other) const {
            return children == other.children && leafs == other.leafs;
        }

        struct Hash {
            size_t operator()(const Node &node) const {
                uint64_t result = 0;
                for (uint i = 0; i < node.children.size(); i++)
                    result ^= (node.children[i] << (i * 4));

                return result;
            }
        };
    };

    static_assert(sizeof(Node) == 36);


    std::vector<Node> dag;
    std::vector<uint32_t> colors;


    CompactOctree() = default;

    void saveToDisk(const std::string &path) {
        std::ofstream output(path, std::ios::out | std::ios::binary);

        output.write(reinterpret_cast<const char *>(dag.data()), sizeof(Node) * dag.size());
    }

    explicit CompactOctree(const std::string &path) {
        std::ifstream input(path, std::ios::in | std::ios::binary);

        Node node;
        while (input.read(reinterpret_cast<char *>(&node), sizeof(Node)))
            dag.push_back(node);
    }
};


struct CompactOctreeBuilder {
    using NodeMap = std::unordered_map<CompactOctree::Node, uint32_t, CompactOctree::Node::Hash>;
    using Color = glm::u8vec3;

    NodeMap nodeMap;
    const RawOctree &raw;

    std::vector<uint32_t> rawColors;
    std::vector<size_t> hashes;

    uint32_t build(uint32_t v, CompactOctree &compact) {
        RawOctree::Node rawNode = raw.data[v];

        if (rawNode.isLeaf())
            rawColors.push_back(rawNode.color);

        CompactOctree::Node newNode{};

        for (uint i = 0; i < 8; i++) {
            auto childOffs = rawNode.children[i];
            if (childOffs != 0) {
                auto child = build(v + childOffs, compact);
                newNode.children[i] = child;
                newNode.leafs += compact.dag[child].leafs;
            }
        }

        if (newNode.leafs == 0) // is a leaf
            newNode.leafs = 1;

//        CompactOctree::Node::Hash hash;
//        hashes.push_back(hash(newNode));
        const auto &res = nodeMap.insert(NodeMap::value_type(newNode, uint32_t(compact.dag.size())));

        if (res.second) {
            compact.dag.push_back(newNode);
        }

        return res.first->second;
    }

    CompactOctree output;

    uint32_t reorder(uint32_t v, const CompactOctree &input, std::vector<uint32_t> &visited) {
        if (visited[v] != 0)
            return visited[v];

        auto node = input.dag[v];

        uint32_t id = output.dag.size();
        output.dag.emplace_back();
        output.dag.back().leafs = node.leafs;

        visited[v] = id;

        for (uint32_t i = 0; i < node.children.size(); i++) {
            auto child = node.children[i];
            if (child != 0) {
                output.dag[id].children[i] = reorder(child, input, visited);
            }
        }

        return id;
    }

    explicit CompactOctreeBuilder(RawOctree &_raw) : raw(_raw) {
        CompactOctree tmp;
        tmp.dag.emplace_back(); // fake node

        std::cout << "Build start" << std::endl;

        uint32_t root = build(0, tmp);

        std::cout << "Built" << std::endl;

        std::vector<uint32_t> visited(tmp.dag.size());
        reorder(root, tmp, visited);

        std::cout << "Reorder end" << std::endl;

//        for (int i = 0; i < 50; i++) {
//            std::cout << hashes[i] << std::endl;
//        }

        output.colors = rawColors;
    }
};