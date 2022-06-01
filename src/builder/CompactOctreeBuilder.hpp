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


    bool compressColors;

    struct ColorBuffer : std::array<uint32_t, 16> {
        uint ptr = 0;

        ColorBuffer() : std::array<uint32_t, 16>() {};
    } buffer;

    CompactOctree::Colors colors {.header = {.compressed = compressColors}};


    void writeColor(uint32_t argb) {
        if (!compressColors) {
            colors.push_back(0); // alpha channel
            colors.push_back(argb >> 16 & 0xff);
            colors.push_back(argb >> 8 & 0xff);
            colors.push_back(argb & 0xff);
        }
        else {
            uint32_t rgba_little_endian =
                    (argb >> 16 & 0xff) | (argb >> 8 & 0xff) << 8 | (argb & 0xff) << 16 | (argb >> 24 & 0xff) << 24;

            buffer[buffer.ptr] = rgba_little_endian;
            buffer.ptr++;

            if (buffer.ptr == buffer.size()) {
                size_t oldSize = colors.size();
                colors.resize(oldSize + 8);
                stb_compress_dxt_block(colors.data() + oldSize, reinterpret_cast<unsigned char *>(buffer.data()), 0,
                                       STB_DXT_NORMAL);

                buffer.ptr = 0;
            }
        }
    }

    std::pair<uint32_t, uint> buildRec(TreeReader<RawOctree::Node> &reader, TempTree &tree, LevelVec &levelVec) {
        RawOctree::Node rawNode = reader.next();

        if (rawNode.isLeaf())
            writeColor(rawNode.color & 0x00ffffff); // pass argb with 0 alpha

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

    void completeColors() {
        while (buffer.ptr != 0)
            writeColor(buffer[buffer.ptr - 1]);

        auto &header = colors.header;
        auto blocks = colors.size() / 8;
        header.blocksX = glm::ceil(glm::sqrt(blocks));

        while ((colors.size() / 8) % header.blocksX != 0) {
            writeColor(0);
        }

        header.blocksY = colors.size() / 8 / header.blocksX;
    }

    static glm::uvec2 mortonDecode(uint32_t id) {
        glm::uvec2 res(0);
        for (int i = 0; i < 32; i++)
            res[i & 1] += (id >> i & 1) << (i >> 1);

        return res;
    }

    static CompactOctree build(const std::filesystem::path &rawOctreePath, bool compressColors) {
        CompactOctreeBuilder builder {.compressColors = compressColors};
        TreeReader<RawOctree::Node> reader(rawOctreePath.string());

        TempTree tempTree;
        tempTree.emplace_back(); // fake node

        {
            LevelVec levelVec;
            builder.buildRec(reader, tempTree, levelVec);
        }

        CompactOctree compactOctree;

        std::vector<uint32_t> visited(tempTree.size());
        builder.reorder(tempTree.size() - 1, tempTree, compactOctree, visited);

        builder.completeColors();
        compactOctree.colors = std::move(builder.colors);

        return compactOctree;
    }


    void dfs(uint v, uint32_t rawID, const CompactOctree &octree, Image<glm::u8vec3> &img, const std::vector<uint32_t> &rawColors) {
        const auto &node = octree.dag[v];

        if (node.isLeaf()) {
            auto coords = mortonDecode(rawID);

            uint32_t color = rawColors[rawID];

            auto &pixel = img.get(coords);
            pixel.x = color >> 16 & 0xff;
            pixel.y = color >> 8 & 0xff;
            pixel.z = color & 0xff;
        } else {
            uint32_t rawChildId = rawID;

            for (unsigned int child : node.children) {
                if (child != 0) {
                    dfs(child, rawChildId, octree, img, rawColors);
                    rawChildId += octree.dag[child].leafs;
                }
            }
        }
    }

    void createImage(const CompactOctree &octree, const std::string &path, const std::vector<uint32_t> &rawColors) {
        int side = glm::ceil(glm::sqrt(rawColors.size()));

        Image<glm::u8vec3> img(side * 2, side * 2, 3);

        dfs(0, 0, octree, img, rawColors);

        img.saveToDisk(path);
    }
};