#include "renderer.h"
#include "raylib.h"

namespace {
    void draw(const std::unordered_set<Voxelizer::Voxel, Voxelizer::VoxelHash> &res) {
        const float GS = Voxelizer::GRID_SIZE;
        const float VS = 1 / GS;

//        for (uint32_t i = 0; i < GS; i++)
//            for (uint32_t j = 0; j < GS; j++)
//                for (uint32_t k = 0; k < GS; k++)
//                    DrawCubeWires({VS * i, VS * j, VS * k}, VS, VS, VS, MAROON);

        for (const auto &v: res) {
            Color c {uint8_t(v.pos.x / GS * 255), uint8_t(v.pos.y / GS * 255), uint8_t(v.pos.z / GS * 255), 255};
            Vector3 pos {v.pos.x * VS, v.pos.y * VS, v.pos.z * VS};
            DrawCube(pos, VS, VS, VS, c);
//            DrawCubeWires(pos, VS, VS, VS, MAROON);
        }

//        DrawCube({1, 1, 1}, VS, VS, VS, RED);

        Vector3 v1{0, 0, 0};
        Vector3 v2{1, 1, 1};
        Vector3 v3{0.9, 0.5, 0.5};

        DrawTriangle3D(v1, v2, v3, RED);
    }
}

int Renderer::render(const std::unordered_set<Voxelizer::Voxel, Voxelizer::VoxelHash> &res) {
    const int screenWidth = 1200;
    const int screenHeight = 800;

    SetTraceLogLevel(LOG_ERROR);
    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera mode");

    Camera3D camera;
    camera.position = Vector3{0.0f, 2.0f, 0.0f};
    camera.target = Vector3{0.0f, 0.0f, 0.0f};
    camera.up = Vector3{0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    SetCameraMode(camera, CAMERA_FIRST_PERSON);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        UpdateCamera(&camera);

        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

            draw(res);

        EndMode3D();

        DrawFPS(10, 10);
        EndDrawing();

    }

    CloseWindow();

    return 0;
}
