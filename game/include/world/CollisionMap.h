#pragma once

#include <vector>

#include "raylib.h"

struct CollisionTriangle {
    Vector3 a;
    Vector3 b;
    Vector3 c;
    BoundingBox bounds;
};

class CollisionMap {
public:
    void BuildFromModel(const Model& model, const Vector3& scale);
    void Clear();

    const std::vector<CollisionTriangle>& GetTriangles() const;

    bool OverlapsAny(const Vector3& center, const Vector3& halfExtents) const;

private:
    std::vector<CollisionTriangle> triangles_;
};
