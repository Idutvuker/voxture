#pragma once

struct Octree {
    using u32 = uint32_t;
    using u64 = uint64_t;
    using Key = uint64_t;

    struct Node {
        std::array<u32, 8> children{};

        bool isLeaf() const {
            for (const auto &child: children)
                if (child != 0)
                    return false;

            return true;
        }

        Node() = default;
    };

    std::vector<Node> data;

    static constexpr u32 MAX_LEVEL = 16;
    static constexpr std::array<uint64_t, MAX_LEVEL> fullTreeSize {1, 9, 73, 585, 4681, 37449, 299593, 2396745, 19173961, 153391689, 1227133513, 9817068105, 78536544841, 628292358729, 5026338869833, 40210710958665};

//    void fillFullTreeSize() {
//        u64 pow = 1;
//        fullTreeSize[0] = 1;
//
//        for (size_t i = 1; i < fullTreeSize.size(); i++) {
//            pow *= 8;
//            fullTreeSize[i] = fullTreeSize[i - 1] + pow;
//        }
//    }

    void dfs(u32 id, std::ostream &output, Key key, u32 level) const {
        const auto &node = data[id];

        u64 writeKey = (u64(level) << 61) | key;

        output.write(reinterpret_cast<const char *>(&key), sizeof(key));

        if (level == MAX_LEVEL)
            return;

        u64 childTreeSize = fullTreeSize[MAX_LEVEL - level - 1];
        u64 childKey = key + 1;

        for (u32 child: node.children) {
            if (child != 0)
                dfs(id + child, output, childKey, level + 1);

            childKey += childTreeSize;
        }
    }

    void saveToDisk(const std::string &filepath) {
        std::ofstream outputFile(filepath, std::ios::out | std::ios::binary);

        if (!outputFile.is_open())
            throw std::runtime_error("Can not open " + filepath);

        dfs(0, outputFile, 0, 0);
    }

    Octree() = default;

    explicit Octree(const std::string &filepath) {
        std::ifstream inputFile(filepath, std::ios::in | std::ios::binary);

        Node node;
        while (inputFile.read(reinterpret_cast<char *>(&node), sizeof(node)))
            data.push_back(node);
    }
};

struct Keys2Tree {
    using u32 = uint32_t;
    using u64 = uint64_t;
    using Key = uint64_t;
    using Node = Octree::Node;
    static constexpr u32 MAX_LEVEL = Octree::MAX_LEVEL;

    static bool readKey(std::istream &input, Key *key, u32 *level) {
        Key paired;
        bool result = bool(input.read(reinterpret_cast<char *>(&paired), sizeof(Key)));

        *key = (paired << 3) << 3;
        *level = paired >> 61;

        return result;
    }

    static void writeNode(std::ostream &ostream, const Node &node) {
        ostream.write(reinterpret_cast<const char*>(&node), sizeof(node));
    }

    static uint findKeyPos(Key key, std::ifstream &input, ) {
        input.seekg();
        return 0;
    }

    static void convert(const std::string &inputFilepath, const std::string &outputFilepath) {
        std::ifstream input(inputFilepath, std::ios::in | std::ios::binary);
        std::ofstream output(outputFilepath, std::ios::out | std::ios::binary);

        if (!input.is_open())
            throw std::runtime_error("Can not open " + inputFilepath);

        if (!output.is_open())
            throw std::runtime_error("Can not open " + outputFilepath);

        Key key;
        u32 level;

        uint pos = 0;
        while (readKey(input, &key, &level)) {
            Node node;

            u64 childTreeSize = Octree::fullTreeSize[MAX_LEVEL - level - 1];
            u64 childKey = key + 1;

            for (u32 i = 0; i < 8; i++) {
                node.children[i] = findKeyPos(childKey) - pos;
                childKey += childTreeSize;
            }

            pos += 1;
        }
    }
};

struct Merger {
    using u32 = uint32_t;
    using Key = uint64_t;

    struct OctreeLoader {
        std::ifstream input;

        explicit OctreeLoader(const std::string &filepath) : input(filepath, std::ios::in | std::ios::binary) {}

        bool getNext(Key *keyPtr) {
            return bool(input.read(reinterpret_cast<char *>(keyPtr), sizeof(Key)));
        }
    };

    static void merge(const std::string &filepath1, const std::string &filepath2, const std::string &outputFilepath)
    {
        OctreeLoader tree1(filepath1);
        OctreeLoader tree2(filepath2);
        std::ofstream output(outputFilepath, std::ios::out | std::ios::binary);

        if (!output.is_open())
            throw std::runtime_error("Can not open " + outputFilepath);

        Key cur1;
        Key cur2;

        bool hasCur1 = tree1.getNext(&cur1);
        bool hasCur2 = tree2.getNext(&cur2);

        while (hasCur1 && hasCur2) {
            Key k;
            if (cur1 < cur2) {
                k = cur1;
                hasCur1 = tree1.getNext(&cur1);
            }
            else if (cur2 < cur1) {
                k = cur2;
                hasCur2 = tree2.getNext(&cur2);
            }
            else {
                k = cur1;
                hasCur1 = tree1.getNext(&cur1);
                hasCur2 = tree2.getNext(&cur2);
            }

            output.write(reinterpret_cast<char *>(&k), sizeof(Key));
        }

        while (hasCur1 || hasCur2) {
            Key k;
            if (hasCur1) {
                k = cur1;
                hasCur1 = tree1.getNext(&cur1);
            } else {
                k = cur2;
                hasCur2 = tree1.getNext(&cur2);
            }
            output.write(reinterpret_cast<char *>(&k), sizeof(Key));
        }
    }
};

//struct Merger {
//    using u32 = uint32_t;
//    using Node = Octree::Node;
//
//    struct OctreeLoader {
//        const Octree octree;
//
//        explicit OctreeLoader(const std::string &filepath) : octree(filepath) {}
//
//        Node getNode(u32 id) const {
//            return octree.data[id];
//        }
//    };
//
//    OctreeLoader tree1;
//    OctreeLoader tree2;
//    std::ofstream output;
//
//    Merger(const std::string &filepath1, const std::string &filepath2, const std::string &outputFilepath) :
//            tree1(filepath1),
//            tree2(filepath2),
//            output(outputFilepath, std::ios::out | std::ios::binary)
//    {
//        if (!output.is_open())
//            throw std::runtime_error("Can not open " + outputFilepath);
//
//        merge(tree1.octree.data.size(), 0, tree2.octree.data.size(), 0);
//    }
//
//    u32 writeSubtree(const OctreeLoader &tree, u32 id, u32 treeSize) {
//        if (id == 0)
//            return 0;
//
//        const Node &node = tree.getNode(id);
//
//        output.write(reinterpret_cast<const char *>(&node), sizeof(node));
//
//        for (u32 child: node.children)
//            if (child != 0)
//                writeSubtree(tree, id + child, treeSize);
//    }
//
//    u32 merge(u32 treeSize1, u32 id1, u32 treeSize2, u32 id2) {
//        const Node &node1 = tree1.getNode(id1);
//        const Node &node2 = tree2.getNode(id2);
//
//        for (uint i = 0; i < 8; i++) {
//            u32 c1Id = node1.children[i];
//            u32 c2Id = node2.children[i];
//
//            if (c1Id == 0)
//                writeSubtree(tree2, c2Id, treeSize2);
//            else if (c2Id == 0)
//                writeSubtree(tree1, c1Id, treeSize1);
//            else {
//                merge(treeSize1, c1Id, treeSize2, c2Id);
//            }
//        }
//        return 0;
//    }
//};
