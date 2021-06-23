#pragma once

#include "main.h"

typedef struct {
    vec3 normal;
    float distance;
} CollisionPacket;

bool IsIntersectingSphereTriangle(CollisionPacket& collisionPacket, vec3 A, vec3 B, vec3 C, vec3 P, float r);
