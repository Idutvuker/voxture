#pragma once

#include <filesystem>
#include <cassert>
#include <array>

struct RawOctree {
    using u32 = uint32_t;

    struct Node {
        std::array<u32, 8> children{};
        u32 color = 0;

        u32 getQuality() const {
            return color >> 24;
        }

        bool isLeaf() const {
            for (const auto &child: children)
                if (child != 0)
                    return false;

            return true;
        }

        Node() = default;
    };

    static_assert(sizeof(Node) == 36);

    std::vector<Node> data;

    RawOctree() = default;

    explicit RawOctree(const std::string &filepath) {
        std::ifstream inputFile(filepath, std::ios::in | std::ios::binary);

        if (!inputFile.is_open())
            throw std::runtime_error("Can not load tree");

        Node node;
        while (inputFile.read(reinterpret_cast<char *>(&node), sizeof(node)))
            data.push_back(node);
    }
};

namespace TreeConst {
    using u32 = uint32_t;
    using u64 = uint64_t;

    static constexpr u32 MAX_LEVEL = 16;

    struct FullTreeSize {
        std::array<u64, MAX_LEVEL + 1> arr;

        constexpr FullTreeSize() : arr() {
            u64 pow = 1;
            arr[0] = 1;

            for (size_t i = 1; i < arr.size(); i++) {
                pow *= 8;
                arr[i] = arr[i - 1] + pow;
            }
        }

        u64 getFromTop(u32 level) const {
            return arr[arr.size() - level - 1];
        }
    };

    constexpr auto fullTreeSize = FullTreeSize();
}

struct DiskKeys {
    using u32 = uint32_t;
    using u64 = uint64_t;
    using Key = uint64_t;
    using Node = RawOctree::Node;

    struct Saver {
        const RawOctree &octree;
        std::ofstream output;

        Saver(const RawOctree &octreeRef, const std::string &outputPath) :
                octree(octreeRef),
                output(outputPath, std::ios::out | std::ios::binary)
        {
            if (!output.is_open())
                throw std::runtime_error("Can not open " + outputPath);
        }

        void save() {
            dfs(0, 0, 0);
        }

        void dfs(u32 id, Key key, u32 level) {
            using namespace TreeConst;

            const auto &node = octree.data[id];

            output.write(reinterpret_cast<const char *>(&key), sizeof(key));

            if (level == MAX_LEVEL)
                return;

            u64 childTreeSize = fullTreeSize.getFromTop(level + 1);
            u64 childKey = key + 1;

            for (u32 child: node.children) {
                if (child != 0)
                    dfs(id + child, childKey, level + 1);

                childKey += childTreeSize;
            }
        }
    };

    struct Converter {
        static bool readKey(std::istream &input, Key *key) {
            return bool(input.read(reinterpret_cast<char *>(key), sizeof(Key)));
        }

        static void writeNode(std::ostream &ostream, const Node &node) {
            ostream.write(reinterpret_cast<const char *>(&node), sizeof(node));
        }

        static u32 findKeyPos(Key key, std::ifstream &input, u32 left, u32 right) {
            Key cur;

            while (right - left > 1) {
                u32 mid = (left + right) / 2;

                input.seekg(mid * int64_t(sizeof(Key)), std::ios::beg);
                readKey(input, &cur);

                if (cur < key) {
                    left = mid;
                } else if (cur > key) {
                    right = mid;
                } else {
                    return mid;
                }
            }
            input.seekg(left * int64_t(sizeof(Key)), std::ios::beg);
            readKey(input, &cur);

            if (cur == key)
                return left;

            return 0;
        }

        static u32 calcLevel(Key key) {
            using namespace TreeConst;

            Key cur = 0;
            u32 level = 0;

            while (cur != key) {
                assert((level != MAX_LEVEL) && "Invalid key");

                Key childTreeSize = fullTreeSize.getFromTop(level + 1);
                Key childKey = cur + 1;

                for (u32 i = 0; i < 8; i++) {
                    if (key < childKey + childTreeSize) {
                        cur = childKey;
                        break;
                    }
                    childKey += childTreeSize;
                }

                level++;
            }

            return level;
        }

        static void convert(const std::string &inputFilepath, const std::string &outputFilepath) {
            using namespace TreeConst;

            std::ifstream input(inputFilepath, std::ios::in | std::ios::binary);
            std::ofstream output(outputFilepath, std::ios::out | std::ios::binary);

            if (!input.is_open())
                throw std::runtime_error("Can not open " + inputFilepath);

            if (!output.is_open())
                throw std::runtime_error("Can not open " + outputFilepath);

            input.seekg(0, std::ios::end);
            u32 length = input.tellg() / sizeof(Key);

            Key key;

            uint pos = 0;
            while (true) {

                input.seekg(pos * int64_t(sizeof(Key)));
                if (!readKey(input, &key))
                    break;

                u32 level = calcLevel(key);

                Node node;

                u64 childTreeSize = fullTreeSize.getFromTop(level + 1);
                u64 childKey = key + 1;

                for (u32 &i: node.children) {
                    u32 childPos = findKeyPos(childKey, input, pos + 1, length);
                    i = childPos != 0 ? childPos - pos : 0;
                    childKey += childTreeSize;
                }

                output.write(reinterpret_cast<const char *>(&node), sizeof(node));

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

        static void merge(const std::string &filepath1, const std::string &filepath2, const std::string &outputFilepath) {
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
                } else if (cur2 < cur1) {
                    k = cur2;
                    hasCur2 = tree2.getNext(&cur2);
                } else {
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
};


