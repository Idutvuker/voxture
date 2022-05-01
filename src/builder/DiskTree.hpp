#pragma once

struct DiskTree {
    using u32 = uint32_t;
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

            dfs(0);
        }

        void dfs(u32 id) {
            using namespace TreeConst;

            const auto &node = octree.data[id];

            output.write(reinterpret_cast<const char *>(&node), sizeof(Node));

            for (u32 child: node.children) {
                if (child != 0)
                    dfs(id + child);
            }
        }
    };

    static void save(const RawOctree &octreeRef, const std::string &outputPath) {
        Saver(octreeRef, outputPath);
    }

    struct Merger {
        struct OctreeLoader {
            std::ifstream input;
            RawOctree octree;

            explicit OctreeLoader(const std::string &filepath) : input(filepath, std::ios::in | std::ios::binary) {}

            Node getNode() {
                Node node;
                input.read(reinterpret_cast<char *>(&node), sizeof(Node));
                return node;
            }
        };

        OctreeLoader tree1;
        OctreeLoader tree2;
        std::ofstream output;

        RawOctree buffer;
        uint bufferLevelThreshold = 4;

        Merger(const std::string &filepath1, const std::string &filepath2, const std::string &outputFilepath) :
                tree1(filepath1),
                tree2(filepath2),
                output(outputFilepath, std::ios::out | std::ios::binary)
        {
            if (!output.is_open())
                throw std::runtime_error("Can not open " + outputFilepath);

            merge(0, 0, 0, 0);
        }


        u32 writeSubtree(OctreeLoader &tree, u32 id, u32 level) {
            assert(id != 0);

            const Node &node = tree.getNode();

            if (level < bufferLevelThreshold) {
                output.write(reinterpret_cast<const char *>(&node), sizeof(Node));
            }
            else {
                buffer.data.push_back(node);
            }

            u32 treeSize = 1;

            for (u32 child: node.children)
                if (child != 0)
                    treeSize += writeSubtree(tree, id + child, level + 1);

            if (level == bufferLevelThreshold) {
                output.write(reinterpret_cast<const char *>(buffer.data.data()), sizeof(Node) * buffer.data.size());
                buffer.data.clear();
            }

            return treeSize;
        }


        u32 merge(u32 id1, u32 id2, u32 pos, u32 level) {
            const Node &node1 = tree1.getNode();
            const Node &node2 = tree2.getNode();

            Node mergedNode{};
            if (node1.getQuality() < node2.getQuality())
                mergedNode.color = node1.color;
            else
                mergedNode.color = node2.color;

            u32 treeSize = 1;

            int64_t writePos;
            if (level < bufferLevelThreshold) {
                writePos = output.tellp();
                output.write(reinterpret_cast<const char *>(&mergedNode), sizeof(Node)); // Useless write, maybe change to seekp
            } else {
                writePos = int64_t(buffer.data.size());
                buffer.data.emplace_back();
            }

            for (uint i = 0; i < 8; i++) {
                u32 t1child = node1.children[i];
                u32 t2child = node2.children[i];

                u32 subTreeSize;

                if (t1child == 0 && t2child == 0)
                    continue;

                if (t1child == 0)
                    subTreeSize = writeSubtree(tree2, id2 + t2child, level + 1);
                else if (t2child == 0)
                    subTreeSize = writeSubtree(tree1, id1 + t1child, level + 1);
                else
                    subTreeSize = merge(id1 + t1child, id2 + t2child, pos + treeSize, level + 1);

                mergedNode.children[i] = treeSize;
                treeSize += subTreeSize;
            }

            if (level < bufferLevelThreshold) {
                auto end = output.tellp();

                output.seekp(writePos);
                output.write(reinterpret_cast<const char *>(&mergedNode), sizeof(Node));

                output.seekp(end);
            } else {
                buffer.data[writePos] = mergedNode;

                if (level == bufferLevelThreshold) {
                    output.write(reinterpret_cast<const char *>(buffer.data.data()), sizeof(Node) * buffer.data.size());
                    buffer.data.clear();
                }

            }

            return treeSize;
        }
    };

    static void merge(const std::string &filepath1, const std::string &filepath2, const std::string &outputFilepath) {
        Merger(filepath1, filepath2, outputFilepath);
    }
};