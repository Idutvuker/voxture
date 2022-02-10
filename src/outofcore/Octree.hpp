#pragma once

struct Octree {
    struct Node {
        std::array<uint32_t, 8> children {};

        bool isLeaf() const {
            for (const auto &child: children)
                if (child != 0)
                    return false;

            return true;
        }

        Node() = default;
    };

    std::vector<Node> data;
};