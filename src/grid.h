//
// Created by Jens on 10-Feb-25.
//

#ifndef GRID_H
#define GRID_H

#include "rlgl.h"
#include "raymath.h"
constexpr float GRID_SIZE = 16.0f;
constexpr float CUBE_SIZE = 0.1f;

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
inline RayCollision CheckCollisionWithGrids(
    const Ray &cameraRay,
    const BoundingBox &bottomGridBoundingBox,
    const BoundingBox &leftGridBoundingBox,
    const BoundingBox &backGridBoundingBox)
{
    RayCollision cameraRayCollision;

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

#endif //GRID_H
