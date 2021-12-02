#pragma once

struct Normalizer {
    static glm::vec3 normalize(std::vector<Triangle> &triangles) {
        auto minAxis = glm::vec3(std::numeric_limits<float>::infinity());
        auto maxAxis = glm::vec3(-std::numeric_limits<float>::infinity());

        for (auto &t: triangles) {
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    minAxis[j] = std::min(minAxis[j], t[i][j]);
                    maxAxis[j] = std::max(maxAxis[j], t[i][j]);
                }
            }
        }

        auto diff = maxAxis - minAxis;

        int scaleAxis = 0;
        for (int i = 1; i < 3; i++)
            if (diff[i] > diff[scaleAxis])
                scaleAxis = i;

        int resolution = 1 << Voxelizer::VOXELIZE_LEVEL;
        float scale = std::nextafter(float(resolution - 1) / float(resolution), 0.f);

        glm::mat4 normalMat = glm::translate(glm::scale(glm::mat4(1), glm::vec3(scale / diff[scaleAxis])), -minAxis);

        for (auto &t: triangles) {
            for (size_t i = 0; i < 3; i++) {
                t[i] = normalMat * glm::vec4(t[i], 1);
//                t[i] = scale * (t[i] - minAxis) / (diff[scaleAxis]);

//                for (int j = 0; j < 3; j++)
//                    assert(0 <= t[i][j] && t[i][j] < 1 && "coordinates should be in [0, 1)");
            }
        }

        glm::vec3 center = (maxAxis - minAxis) / 2.f / diff[scaleAxis];

        return center;
    }
};