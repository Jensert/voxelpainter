#include <iostream>
#include <sstream>
#include <math.h>

#include "raylib.h"
#include "raymath.h"
#include "grid.h"

typedef struct App {
    static constexpr int renderWidth = 640;
    static constexpr int renderHeight = 360;
    int windowWidth = 960;
    int windowHeight = 540;

    bool debug = true;
    bool camera_enabled = true;
    bool mouse_shown = false;

    Voxel voxelGrid[(int)GRID_SIZE*(int)GRID_SIZE*(int)GRID_SIZE];

    void DrawCrosshair() const {
        DrawLine(this->windowWidth/2 - 5, this->windowHeight/2 - 5, this->windowWidth/2 + 5, this->windowHeight/2 + 5, BLACK );
        DrawLine(this->windowWidth/2 - 5, this->windowHeight/2 + 5, this->windowWidth/2 + 5, this->windowHeight/2 - 5, BLACK );
    }

    void DrawDebug() const {
        DrawBoundingBox(GetBackGridBoundingBox(), RED);
        DrawBoundingBox(GetBottomGridBoundingBox(), RED);
        DrawBoundingBox(GetLeftGridBoundingBox(), RED);

        for (int i = 0; i < GRID_SIZE * GRID_SIZE * GRID_SIZE; i++) {
            if (!this->voxelGrid[i].isActive) {
                continue;
            }
            DrawBoundingBox(this->voxelGrid[i].boundingBox, RED);
        }
    }
} App;

App app;

int GetVoxelIndex(int x, int y, int z) {
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE || z < 0 || z >= GRID_SIZE)
        return -1; // Out of bounds

    return x + y * GRID_SIZE + z * (GRID_SIZE * GRID_SIZE);
}

Voxel *GetVoxel(float worldX, float worldY, float worldZ) {
    int voxelX = floorf(worldX / CUBE_SIZE);  // Use floorf to ensure consistent rounding
    int voxelY = floorf(worldY / CUBE_SIZE);
    int voxelZ = floorf(abs(worldZ) / CUBE_SIZE);

    int index = GetVoxelIndex(voxelX, voxelY, voxelZ);
    if (index == -1) return NULL; // Out of bounds

    return &app.voxelGrid[index];
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(app.windowWidth, app.windowHeight, "Voxel painter");
    HideCursor();

    Camera camera = { 0 };
    camera.position = Vector3{GRID_SIZE, GRID_SIZE, GRID_SIZE}*CUBE_SIZE;
    camera.target = Vector3{0.0f, 0.0f, 0.0f};
    camera.up = Vector3{0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    const int cameraMode = CAMERA_FREE;

    const BoundingBox bottomGridBoundingBox = GetBottomGridBoundingBox();
    const BoundingBox leftGridBoundingBox = GetLeftGridBoundingBox();
    const BoundingBox backGridBoundingBox = GetBackGridBoundingBox();

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {

        // Camera
        // ------
        const Vector3 cameraDirection = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
        Ray cameraRay;
        cameraRay.position = camera.position;
        cameraRay.direction = cameraDirection;
        const RayCollision cameraRayCollision = CheckAllCollisions(cameraRay, app.voxelGrid);
        if (app.camera_enabled) {
            UpdateCamera(&camera, cameraMode);
            SetMousePosition(app.windowWidth/2, app.windowHeight/2);
        }

        BeginDrawing();
            ClearBackground(SKYBLUE);
            BeginMode3D(camera);

                // DRAW THE GRID
                DrawGridBottom();
                DrawGridLeft();
                DrawGridBack();

                // DRAW ALL THE PLACED VOXELS
                for (int i = 0; i < GRID_SIZE * GRID_SIZE * GRID_SIZE; i++) {
                    if (!app.voxelGrid[i].isActive) {
                        continue;
                    }
                    DrawCube(app.voxelGrid[i].position, CUBE_SIZE, CUBE_SIZE, CUBE_SIZE, app.voxelGrid[i].color);
                }

                // DRAW THE CURRENT VOXEL
                DrawCube(
                    cameraRayCollision.point,
                    CUBE_SIZE, CUBE_SIZE, CUBE_SIZE, BLUE);
                DrawBoundingBox(returnRayCollisionBoundingBox(cameraRayCollision), BLACK);

                // DRAW DEBUG INFO
                if (app.debug) {
                    app.DrawDebug();
                }

            EndMode3D();
            app.DrawCrosshair();
            DrawFPS(10,10);
            DrawText(TextFormat("Coords: %.2f %.2f %.2f", cameraRayCollision.point.x, cameraRayCollision.point.y, cameraRayCollision.point.z), 10, 30, 20, LIME);
            DrawText(TextFormat("Normal: %.2f %.2f %.2f", cameraRayCollision.normal.x, cameraRayCollision.normal.y, cameraRayCollision.normal.z), 10, 50, 20, LIME);
        EndDrawing();

        CheckCollisionWithVoxels(cameraRay, app.voxelGrid);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && app.camera_enabled) {
            if (Voxel *selectedVoxel = GetVoxel(cameraRayCollision.point.x, cameraRayCollision.point.y, cameraRayCollision.point.z)) {
                selectedVoxel->color = BLUE;
                selectedVoxel->position = cameraRayCollision.point;
                selectedVoxel->boundingBox = returnRayCollisionBoundingBox(cameraRayCollision);
                selectedVoxel->isActive = true;
            }
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        }
        if (IsKeyPressed(KEY_LEFT_ALT)) {
            app.camera_enabled = !app.camera_enabled;
            app.mouse_shown = !app.mouse_shown;
            if (app.camera_enabled) {
                HideCursor();
            } else {
                ShowCursor();
            }
        }
        if (IsKeyPressed(KEY_F11)) {
            MaximizeWindow();
        }
        if (IsKeyPressed(KEY_F1)) {
            app.debug = !app.debug;
        }
        if (IsWindowResized()) {
            app.windowWidth = GetScreenWidth();
            app.windowHeight = GetScreenHeight();
        }
    }
    CloseWindow();
    return 0;
}