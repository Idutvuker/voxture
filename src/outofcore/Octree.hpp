#pragma once

struct Octree {
    using u32 = uint32_t;

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

    void dfs(u32 id, std::ostream &output) const {
        const auto &node = data[id];

        output.write(reinterpret_cast<const char *>(&node), sizeof(node));

        for (u32 child: node.children)
            if (child != 0)
                dfs(id + child, output);
    }

    void saveToDisk(const std::string &filepath) const {
        std::ofstream outputFile(filepath, std::ios::out | std::ios::binary);

        if (!outputFile.is_open())
            throw std::runtime_error("Can not open " + filepath);

        dfs(0, outputFile);
    }

    Octree() = default;

    explicit Octree(const std::string &filepath) {
        assert(sizeof(Node) == 32);

        std::ifstream inputFile(filepath, std::ios::in | std::ios::binary);

        Node node;
        while (inputFile.read(reinterpret_cast<char *>(&node), sizeof(node)))
            data.push_back(node);
    }
};

struct Merger {
    using u32 = uint32_t;
    using Node = Octree::Node;

    struct OctreeLoader {
        const Octree octree;

        explicit OctreeLoader(const std::string &filepath) : octree(filepath) {}

        Node getNode(u32 id) const {
            return octree.data[id];
        }
    };

    OctreeLoader tree1;
    OctreeLoader tree2;
    std::ofstream output;

    Merger(const std::string &filepath1, const std::string &filepath2, const std::string &outputFilepath) :
            tree1(filepath1),
            tree2(filepath2),
            output(outputFilepath, std::ios::out | std::ios::binary)
    {
        if (!output.is_open())
            throw std::runtime_error("Can not open " + outputFilepath);

        merge(tree1.octree.data.size(), 0, tree2.octree.data.size(), 0);
    }

    u32 merge(u32 treeSize1, u32 id1, u32 treeSize2, u32 id2) {
        const Node &node1 = tree1.getNode(id1);
        const Node &node2 = tree2.getNode(id2);

        for (uint i = 0; i < 8; i++) {
            u32 c1Id = node1.children[i];
            u32 c2Id = node2.children[i];

            if (c1Id == 0) {
                output.write()
            }
        }

    }
};