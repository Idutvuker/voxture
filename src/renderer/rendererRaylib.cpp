#include "rendererRaylib.h"
#include <raylib/raylib.h>
#include <raylib/rlgl.h>

namespace {
    Color randomColor() {
        return Color{uint8_t(GetRandomValue(0, 255)), uint8_t(GetRandomValue(0, 255)), uint8_t(GetRandomValue(0, 255)), 255};
    }

    Vector3 g2r(const glm::vec3 &v) {
        return Vector3{v.x, v.y, v.z};
    }

    void draw(const std::vector<Triangle> &triangles,
              const Voxelizer::VoxelSet &voxelSet) {

        using namespace glm;
        using u8 = uint8_t;

        const auto GS = float(voxelSet.getGridSize());
        const float VS = 1 / GS;

//        for (uint32_t i = 0; i < GS; i++)
//            for (uint32_t j = 0; j < GS; j++)
//                for (uint32_t k = 0; k < GS; k++)
//                    DrawCubeWires({VS * i, VS * j, VS * k}, VS, VS, VS, MAROON);

        for (const auto &v: voxelSet.set) {
            Color c {uint8_t(v.pos.x / GS * 255), uint8_t(v.pos.y / GS * 255), uint8_t(v.pos.z / GS * 255), 255};
            Vector3 pos = g2r(vec3(v.pos) * VS + VS / 2);
            DrawCube(pos, VS, VS, VS, c);
//            DrawCubeWires(pos, VS, VS, VS, MAROON);
        }

//        DrawCube({1, 1, 1}, VS, VS, VS, RED);
        DrawLine3D({0, 0, 0}, {1, 0, 0}, RED);
        DrawLine3D({0, 0, 0}, {0, 0, 1}, GREEN);

        static bool drawModel = false;
        if (IsKeyPressed(KEY_SPACE))
            drawModel ^= 1;

        if (drawModel) {
            for (const auto &tri: triangles) {
                DrawTriangle3D(g2r(tri.a), g2r(tri.b), g2r(tri.c), randomColor());
//                DrawLine3D(g2r(tri.a), g2r(tri.b), WHITE);
//                DrawLine3D(g2r(tri.b), g2r(tri.c), WHITE);
//                DrawLine3D(g2r(tri.c), g2r(tri.a), WHITE);
            }
        }

    }
}

int RendererRaylib::render(const std::vector<Triangle> &triangles,
                           const Voxelizer::OctreeLevels &treeLevels) {
    const int screenWidth = 1200;
    const int screenHeight = 800;

    SetTraceLogLevel(LOG_ERROR);
    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera mode");

    Camera3D camera;
    camera.position = Vector3{0.0f, 0.8f, 0.0f};
    camera.target = Vector3{0.0f, 0.0f, 0.0f};
    camera.up = Vector3{0.0f, 1.0f, 0.0f};
    camera.fovy = 80.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    SetCameraMode(camera, CAMERA_FIRST_PERSON);

    SetTargetFPS(60);

    rlDisableBackfaceCulling();

    size_t curLevel = 0;

    while (!WindowShouldClose()) {
        UpdateCamera(&camera);

        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        if (IsKeyPressed(KEY_UP)) {
            curLevel = std::min(curLevel + 1, treeLevels.data.size());
        }
        if (IsKeyPressed(KEY_DOWN)) {
            if (curLevel > 0)
                curLevel--;
        }


        draw(triangles, treeLevels.data[curLevel]);

        EndMode3D();

        DrawFPS(10, 10);
        EndDrawing();

    }

    CloseWindow();

    return 0;
}
