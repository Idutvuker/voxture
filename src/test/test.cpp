#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>

#include <fstream>
#include <iostream>


template<typename T>
void pp(const std::string &tag, const T &val) {
    std::cout << tag << glm::to_string(val) << std::endl;
}

template<typename T>
void pp(const T &val) {
    std::cout << glm::to_string(val) << std::endl;
}

void loadCameras() {
    using namespace glm;

    std::string filepath = "resources/saharov/cameras.out";
    std::ifstream filestream(filepath);

    std::string header_line;
    std::getline(filestream, header_line);
    if (header_line != "# Bundle file v0.3") {
        throw std::runtime_error("Can't import bundler .out file - unexpected header line! " + header_line);
    }

    size_t nCameras, nPoints;
    filestream >> nCameras >> nPoints;

    for (size_t c = 0; c < 1; c++) {
        double f, k1, k2;
        filestream >> f >> k1 >> k2;

        glm::mat3 orientation;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                filestream >> orientation[i][j];
            }
        }

        std::swap(orientation[1], orientation[2]);
//        orientation[1] = -orientation[1];

        orientation = glm::transpose(orientation);


//        for (int i = 0; i < 3; i++)
//            std::swap(orientation[i][1], orientation[i][2]);

        pp(orientation[0]);
        pp(orientation[1]);
        pp(orientation[2]);

        printf("vec: ");
        pp(orientation * vec3(0, 0, 1));
//
//        vec3 p = vec3(1, 1, 2);//vec3(1.996026, -0.227806, -1.401422)
//        pp(orientation * p);
//        printf("\n\n");
//

//
//        pp(orientation[0]);
//        pp(orientation[1]);
//        pp(orientation[2]);
//
//        pp(orientation * p);


        glm::vec3 pos;
        for (int i = 0; i < 3; i++) {
            filestream >> pos[i];
        }

        std::swap(pos[1], pos[2]);
        pos[2] = -pos[2];

        pos = orientation * (-pos);
//
        printf("pos: ");
        pp(pos);

//        vec3 euler;
//        extractEulerAngleXYZ(mat4(orientation), euler.x, euler.y, euler.z);
//        pp(euler / pi<float>() * 180.f);

//        glm::mat4 result = mat4(orientation);// * translate(mat4(1), pos);
    }
}

int mainTest() {
    loadCameras();
    return 0;
}