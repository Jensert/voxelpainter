//
// Created by Jens on 10-Feb-25.
//

#ifndef GRID_H
#define GRID_H

#include <cfloat>

#include "rlgl.h"
#include "raymath.h"

constexpr float GRID_SIZE = 16.0f;
constexpr float CUBE_SIZE = 0.25f;

struct Voxel {
    Vector3 position;
    Color color;
    BoundingBox boundingBox;
    bool isActive = false;  // Track whether voxel is used
};

enum DrawMode {
    DRAWMODE_FPS = 1,
    DRAWMODE_SELECT = 2,
};

inline void DrawGridBottom()
{
    const float offset = static_cast<float>(GRID_SIZE) * CUBE_SIZE;

    rlBegin(RL_LINES);
    for (int i = 0; i <= GRID_SIZE; i++)
    {
        if (i == 0)
        {
            rlColor3f(0.5f, 0.5f, 0.5f);
        }
        else
        {
            rlColor3f(0.75f, 0.75f, 0.75f);
        }

        const float startPos = static_cast<float>(i) * CUBE_SIZE;

        // Vertical lines
        rlVertex3f(startPos, 0.0f, offset); // Front vertex
        rlVertex3f(startPos, 0.0f, 0.0f); // Back vertex

        // Horizontal lines
        rlVertex3f(0.0f, 0.0f, startPos); // Left vertex
        rlVertex3f(offset, 0.0f, startPos); // Right vertex
    }
    rlEnd();
}

inline void DrawGridLeft()
{
    const float offset = static_cast<float>(GRID_SIZE) * CUBE_SIZE;

    rlBegin(RL_LINES);
    for (int i = 0; i <= GRID_SIZE; i++)
    {
        if (i == 0)
        {
            rlColor3f(0.5f, 0.5f, 0.5f);
        }
        else
        {
            rlColor3f(0.75f, 0.75f, 0.75f);
        }

        const float startPos = static_cast<float>(i) * CUBE_SIZE;

        // Vertical lines
        rlVertex3f(0.0f, startPos, 0.0f); // Back vertex
        rlVertex3f(0.0f, startPos, offset); // Front vertex

        // Horizontal lines
        rlVertex3f(0.0f, 0.0f, startPos); // Bottom vertex
        rlVertex3f(0.0f, offset, startPos); // Top vertex
    }
    rlEnd();
}

inline void DrawGridBack()
{
    const float offset = static_cast<float>(GRID_SIZE) * CUBE_SIZE;

    rlBegin(RL_LINES);
    for (int i = 0; i <= GRID_SIZE; i++)
    {
        if (i == 0)
        {
            rlColor3f(0.5f, 0.5f, 0.5f);
        }
        else
        {
            rlColor3f(0.75f, 0.75f, 0.75f);
        }

        const float startPos = static_cast<float>(i) * CUBE_SIZE;

        // Vertical lines
        rlVertex3f(startPos, 0.0f, 0); // Bottom vertex
        rlVertex3f(startPos, offset, 0); // Top vertex

        // Horizontal lines
        rlVertex3f(0.0f, startPos, 0); // Left vertex
        rlVertex3f(offset, startPos, 0); // Right vertex
    }
    rlEnd();
}

inline BoundingBox GetBottomGridBoundingBox() {
    const float gridWidth = static_cast<float>(GRID_SIZE) * CUBE_SIZE;

    BoundingBox bbox = {
        .min = {0.0f, 0.0f, 0.0f}, // Bottom-left corner of the grid
        .max = {gridWidth, 0.0f, gridWidth}   // Top-right corner of the grid
    };

    return bbox;
}

inline BoundingBox GetLeftGridBoundingBox() {
    const float gridWidth = static_cast<float>(GRID_SIZE) * CUBE_SIZE;

    BoundingBox bbox = {
        .min = {0.0f, 0.0f, 0.0f},   // Top-right corner of the grid
        .max = {0.0f, gridWidth, gridWidth} // Bottom-left corner of the grid
    };

    return bbox;
}

inline BoundingBox GetBackGridBoundingBox() {
    const float gridWidth = static_cast<float>(GRID_SIZE) * CUBE_SIZE;

    BoundingBox bbox = {
        .min = {0.0f, 0.0f, 0.0f},  // Bottom-left (XY plane at fixed Z)
        .max = {gridWidth, gridWidth, 0}  // Top-right (XY plane at fixed Z)
    };

    return bbox;
}

// Function to check all grid collisions
inline RayCollision CheckCollisionWithGrids(const Ray &cameraRay)
{
    RayCollision cameraRayCollision;
    const BoundingBox bottomGridBoundingBox = GetBottomGridBoundingBox();
    const BoundingBox leftGridBoundingBox = GetLeftGridBoundingBox();
    const BoundingBox backGridBoundingBox = GetBackGridBoundingBox();

    // Initialize to an empty collision (no collision)
    cameraRayCollision.hit = false;

    // Check collision with the bottom grid
    if (RayCollision bottomCollision = GetRayCollisionBox(cameraRay, bottomGridBoundingBox); bottomCollision.hit) {
        // If hit, snap to grid
        bottomCollision.point.x = (floorf(bottomCollision.point.x / CUBE_SIZE) * CUBE_SIZE) + CUBE_SIZE / 2;
        bottomCollision.point.z = (floorf(bottomCollision.point.z / CUBE_SIZE) * CUBE_SIZE) + CUBE_SIZE / 2;
        bottomCollision.point.y = (floorf(bottomCollision.point.y / CUBE_SIZE) * CUBE_SIZE) + CUBE_SIZE / 2;
        cameraRayCollision = bottomCollision;
    }

    // Check collision with the left grid
    if (RayCollision leftCollision = GetRayCollisionBox(cameraRay, leftGridBoundingBox); leftCollision.hit) {
        // If hit, snap to grid
        leftCollision.point.x = (floorf(leftCollision.point.x / CUBE_SIZE) * CUBE_SIZE) + CUBE_SIZE / 2;
        leftCollision.point.y = (floorf(leftCollision.point.y / CUBE_SIZE) * CUBE_SIZE) + CUBE_SIZE / 2;
        leftCollision.point.z = (floorf(leftCollision.point.z / CUBE_SIZE) * CUBE_SIZE) + CUBE_SIZE / 2;
        cameraRayCollision = leftCollision;
    }

    // Check collision with the back grid
    if (RayCollision backCollision = GetRayCollisionBox(cameraRay, backGridBoundingBox); backCollision.hit) {
        // If hit, snap to grid
        backCollision.point.x = (floorf(backCollision.point.x / CUBE_SIZE) * CUBE_SIZE) + CUBE_SIZE / 2;
        backCollision.point.y = (floorf(backCollision.point.y / CUBE_SIZE) * CUBE_SIZE) + CUBE_SIZE / 2;
        backCollision.point.z = (floorf(backCollision.point.z / CUBE_SIZE) * CUBE_SIZE) + CUBE_SIZE / 2;
        cameraRayCollision = backCollision;
    }

    cameraRayCollision.point.x = Clamp(cameraRayCollision.point.x, 0.0f, CUBE_SIZE * GRID_SIZE);
    cameraRayCollision.point.y = Clamp(cameraRayCollision.point.y, 0.0f, CUBE_SIZE * GRID_SIZE);
    cameraRayCollision.point.z = Clamp(cameraRayCollision.point.z, 0.0f, CUBE_SIZE * GRID_SIZE);

    cameraRayCollision.point = Vector3{
        floorf(cameraRayCollision.point.x / CUBE_SIZE) * CUBE_SIZE + CUBE_SIZE / 2,
        floorf(cameraRayCollision.point.y / CUBE_SIZE) * CUBE_SIZE + CUBE_SIZE / 2,
        floorf(cameraRayCollision.point.z / CUBE_SIZE) * CUBE_SIZE + CUBE_SIZE / 2};
    return cameraRayCollision;
}

// Function to check all grid collisions
inline RayCollision CheckCollisionWithVoxels(
    const Ray &cameraRay,
    const Voxel voxelGrid[])
{
    RayCollision closestCollision;
    closestCollision.hit = false;
    closestCollision.distance = FLT_MAX;

    for (int i = 0; i < GRID_SIZE*GRID_SIZE*GRID_SIZE; i++) {
        if (!voxelGrid[i].isActive) continue;
        RayCollision collision = GetRayCollisionBox(cameraRay, voxelGrid[i].boundingBox);

        if (collision.hit && collision.distance < closestCollision.distance) {
            closestCollision = collision;
        }
    }

    closestCollision.point += (closestCollision.normal * CUBE_SIZE/2);
    closestCollision.point = Vector3{
        floorf(closestCollision.point.x / CUBE_SIZE) * CUBE_SIZE + CUBE_SIZE / 2,
        floorf(closestCollision.point.y / CUBE_SIZE) * CUBE_SIZE + CUBE_SIZE / 2,
        floorf(closestCollision.point.z / CUBE_SIZE) * CUBE_SIZE + CUBE_SIZE / 2};

    return closestCollision;
}

inline RayCollision CheckAllCollisions(const Ray &cameraRay, const Voxel voxelGrid[]) {
    RayCollision gridCollision = CheckCollisionWithGrids(cameraRay);
    RayCollision voxelCollision = CheckCollisionWithVoxels(cameraRay, voxelGrid);
    if (voxelCollision.hit) {
        return voxelCollision;
    }
    return gridCollision;
}

inline void PrintActiveVoxelIndexes(Voxel voxelGrid[]) {
    for (int i = 0; i < GRID_SIZE*GRID_SIZE*GRID_SIZE; i++) {
        if (!voxelGrid[i].isActive) continue;
        std::cout << i << std::endl;
    }
}

inline BoundingBox returnRayCollisionBoundingBox(const RayCollision &rayCollision) {
    return BoundingBox {
        .min = {
            rayCollision.point.x - CUBE_SIZE/2,
            rayCollision.point.y - CUBE_SIZE/2,
            rayCollision.point.z - CUBE_SIZE/2,
        },
        .max = {
            rayCollision.point.x + CUBE_SIZE/2,
            rayCollision.point.y + CUBE_SIZE/2,
            rayCollision.point.z + CUBE_SIZE/2,
        },
    };
}

#endif //GRID_H
