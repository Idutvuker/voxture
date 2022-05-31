#pragma once

#include <array>

struct CompactOctreeBuilder {
    struct Node {
        size_t hash = 0;
        uint32_t leafs = 0;
        std::array<uint32_t, 8> children {};

        bool isLeaf() const {
            for (const auto &child: children)
                if (child != 0)
                    return false;

            return true;
        }

        template <class T>
        static void hash_combine(std::size_t& seed, const T& v)
        {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }

        void calcHash() {
            hash = 0;
            for (unsigned int & i : children)
                hash_combine(hash, i);

            hash_combine(hash, leafs);
        }

        bool operator==(const Node &other) const {
            return children == other.children;  //hash == other.hash;
        }

        struct Hash {
            size_t operator()(const Node &node) const {
                return node.hash;
            }
        };
    };

    using TempTree = std::vector<Node>;

    using NodeMap = std::unordered_map<Node, uint32_t, Node::Hash>;
    using LevelVec = std::vector<NodeMap>;

    std::array<uint32_t, 16> buffer;
    uint bufferPtr = 0;

    std::vector<uint8_t> colors;

    void writeColor(uint32_t rgb) {
        buffer[bufferPtr] = rgb << 8; // rgba, alpha is const zero
        bufferPtr += 1;

        if (bufferPtr == buffer.size()) {
            size_t oldSize = colors.size();
            colors.resize(oldSize + 8);
            stb_compress_dxt_block(colors.data() + oldSize, reinterpret_cast<unsigned char *>(buffer.data()), 0, STB_DXT_NORMAL);

            bufferPtr = 0;
        }
    }

    std::pair<uint32_t, uint> buildRec(TreeReader<RawOctree::Node> &reader, TempTree &tree, LevelVec &levelVec) {
        RawOctree::Node rawNode = reader.next();

        if (rawNode.isLeaf())
            writeColor(rawNode.color);

        Node newNode{};
        uint height = 0;

        for (uint i = 0; i < 8; i++) {
            auto childOffs = rawNode.children[i];
            if (childOffs != 0) {
                auto child = buildRec(reader, tree, levelVec);
                newNode.children[i] = child.first;
                newNode.leafs += tree[child.first].leafs;

                height = std::max(height, child.second + 1);
            }
        }

        if (newNode.leafs == 0) // is a leaf
            newNode.leafs = 1;

        newNode.calcHash();

        if (height == levelVec.size())
            levelVec.emplace_back();

        auto &nodeMap = levelVec.at(height);

        if (height == 0 && !nodeMap.empty()) {
            return {1, height};
        }

        const auto &res = nodeMap.insert(NodeMap::value_type(newNode, uint32_t(tree.size())));
        if (res.second) {
            tree.push_back(newNode);
        }

        return {res.first->second, height};
    }

    uint32_t reorder(uint32_t v, const TempTree &input, CompactOctree &output, std::vector<uint32_t> &visited) {
        if (visited[v] != 0)
            return visited[v];

        auto node = input[v];

        uint32_t id = output.dag.size();
        output.dag.emplace_back();
        output.dag.back().leafs = node.leafs;

        visited[v] = id;

        for (uint32_t i = 0; i < node.children.size(); i++) {
            auto child = node.children[i];
            if (child != 0) {
                output.dag[id].children[i] = reorder(child, input, output, visited);
            }
        }

        return id;
    }

    static CompactOctree build(const std::filesystem::path &rawOctreePath) {
        CompactOctreeBuilder builder;
        TreeReader<RawOctree::Node> reader(rawOctreePath.string());

        std::vector<uint8_t> colors;

        TempTree tempTree;
        tempTree.emplace_back(); // fake node

        {
            LevelVec levelVec;
            builder.buildRec(reader, tempTree, levelVec);
        }

        CompactOctree compactOctree;

        std::vector<uint32_t> visited(tempTree.size());
        builder.reorder(tempTree.size() - 1, tempTree, compactOctree, visited);

        compactOctree.colors = colors;

        return compactOctree;
    }
};