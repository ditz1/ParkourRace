#include "world/CollisionMap.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

constexpr float kMinAxisThickness = 0.04f;
constexpr float kDedupQuantizeScale = 1000.0f;
constexpr float kCollisionEpsilon = 0.00001f;

Vector3 LoadScaledVertex(const Mesh& mesh, int index, const Vector3& scale) {
    return {
        mesh.vertices[(index * 3) + 0] * scale.x,
        mesh.vertices[(index * 3) + 1] * scale.y,
        mesh.vertices[(index * 3) + 2] * scale.z,
    };
}

float Dot(const Vector3& a, const Vector3& b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

Vector3 Sub(const Vector3& a, const Vector3& b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vector3 Add(const Vector3& a, const Vector3& b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

Vector3 Mul(const Vector3& a, float s) {
    return {a.x * s, a.y * s, a.z * s};
}

float DistSq(const Vector3& a, const Vector3& b) {
    const Vector3 d = Sub(a, b);
    return Dot(d, d);
}

Vector3 ClampPointToBounds(const Vector3& p, const BoundingBox& b) {
    auto clamp = [](float v, float minv, float maxv) -> float {
        return std::max(minv, std::min(v, maxv));
    };

    return {
        clamp(p.x, b.min.x, b.max.x),
        clamp(p.y, b.min.y, b.max.y),
        clamp(p.z, b.min.z, b.max.z),
    };
}

bool SphereIntersectsBounds(const Vector3& center, float radius, const BoundingBox& b) {
    const Vector3 closest = ClampPointToBounds(center, b);
    return DistSq(center, closest) <= (radius * radius);
}

void EnsureMinThickness(BoundingBox& box) {
    const float xExtent = box.max.x - box.min.x;
    const float yExtent = box.max.y - box.min.y;
    const float zExtent = box.max.z - box.min.z;

    if (xExtent < kMinAxisThickness) {
        const float cx = (box.min.x + box.max.x) * 0.5f;
        box.min.x = cx - (kMinAxisThickness * 0.5f);
        box.max.x = cx + (kMinAxisThickness * 0.5f);
    }
    if (yExtent < kMinAxisThickness) {
        const float cy = (box.min.y + box.max.y) * 0.5f;
        box.min.y = cy - (kMinAxisThickness * 0.5f);
        box.max.y = cy + (kMinAxisThickness * 0.5f);
    }
    if (zExtent < kMinAxisThickness) {
        const float cz = (box.min.z + box.max.z) * 0.5f;
        box.min.z = cz - (kMinAxisThickness * 0.5f);
        box.max.z = cz + (kMinAxisThickness * 0.5f);
    }
}

BoundingBox MakeTriangleBounds(const Vector3& a, const Vector3& b, const Vector3& c) {
    BoundingBox box{};
    box.min = {
        std::min(a.x, std::min(b.x, c.x)),
        std::min(a.y, std::min(b.y, c.y)),
        std::min(a.z, std::min(b.z, c.z)),
    };
    box.max = {
        std::max(a.x, std::max(b.x, c.x)),
        std::max(a.y, std::max(b.y, c.y)),
        std::max(a.z, std::max(b.z, c.z)),
    };
    EnsureMinThickness(box);
    return box;
}

std::string QuantizedVertexKey(const Vector3& v) {
    const int qx = static_cast<int>(std::lround(v.x * kDedupQuantizeScale));
    const int qy = static_cast<int>(std::lround(v.y * kDedupQuantizeScale));
    const int qz = static_cast<int>(std::lround(v.z * kDedupQuantizeScale));
    return std::to_string(qx) + "|" + std::to_string(qy) + "|" + std::to_string(qz);
}

std::string MakeTriangleKey(const Vector3& a, const Vector3& b, const Vector3& c) {
    std::string keys[3] = {QuantizedVertexKey(a), QuantizedVertexKey(b), QuantizedVertexKey(c)};
    std::sort(keys, keys + 3);
    return keys[0] + "#" + keys[1] + "#" + keys[2];
}

Vector3 ClosestPointOnTriangle(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c) {
    // Real-Time Collision Detection (Christer Ericson), closest point on triangle.
    const Vector3 ab = Sub(b, a);
    const Vector3 ac = Sub(c, a);
    const Vector3 ap = Sub(p, a);
    const float d1 = Dot(ab, ap);
    const float d2 = Dot(ac, ap);
    if (d1 <= 0.0f && d2 <= 0.0f) return a;

    const Vector3 bp = Sub(p, b);
    const float d3 = Dot(ab, bp);
    const float d4 = Dot(ac, bp);
    if (d3 >= 0.0f && d4 <= d3) return b;

    const float vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
        const float v = d1 / (d1 - d3);
        return Add(a, Mul(ab, v));
    }

    const Vector3 cp = Sub(p, c);
    const float d5 = Dot(ab, cp);
    const float d6 = Dot(ac, cp);
    if (d6 >= 0.0f && d5 <= d6) return c;

    const float vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
        const float w = d2 / (d2 - d6);
        return Add(a, Mul(ac, w));
    }

    const float va = d3 * d6 - d5 * d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
        const Vector3 bc = Sub(c, b);
        const float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return Add(b, Mul(bc, w));
    }

    const float denom = 1.0f / (va + vb + vc);
    const float v = vb * denom;
    const float w = vc * denom;
    return Add(a, Add(Mul(ab, v), Mul(ac, w)));
}

bool SphereIntersectsTriangle(const Vector3& center, float radius, const CollisionTriangle& tri) {
    if (!SphereIntersectsBounds(center, radius, tri.bounds)) {
        return false;
    }

    const Vector3 closest = ClosestPointOnTriangle(center, tri.a, tri.b, tri.c);
    return DistSq(center, closest) <= ((radius * radius) + kCollisionEpsilon);
}

void AddBoxesFromMesh(const Mesh& mesh,
                      const Vector3& scale,
                      std::vector<CollisionTriangle>& outTriangles,
                      std::unordered_set<std::string>& dedupSet) {
    if (mesh.vertexCount <= 0 || mesh.vertices == nullptr) {
        return;
    }

    if (mesh.indices != nullptr && mesh.triangleCount > 0) {
        for (int t = 0; t < mesh.triangleCount; ++t) {
            const int i0 = static_cast<int>(mesh.indices[(t * 3) + 0]);
            const int i1 = static_cast<int>(mesh.indices[(t * 3) + 1]);
            const int i2 = static_cast<int>(mesh.indices[(t * 3) + 2]);
            if (i0 < 0 || i1 < 0 || i2 < 0 || i0 >= mesh.vertexCount || i1 >= mesh.vertexCount || i2 >= mesh.vertexCount) {
                continue;
            }
            const Vector3 v0 = LoadScaledVertex(mesh, i0, scale);
            const Vector3 v1 = LoadScaledVertex(mesh, i1, scale);
            const Vector3 v2 = LoadScaledVertex(mesh, i2, scale);
            const std::string key = MakeTriangleKey(v0, v1, v2);
            if (dedupSet.insert(key).second) {
                outTriangles.push_back({v0, v1, v2, MakeTriangleBounds(v0, v1, v2)});
            }
        }
    } else {
        const int triVertCount = (mesh.vertexCount / 3) * 3;
        for (int i = 0; i < triVertCount; i += 3) {
            const Vector3 v0 = LoadScaledVertex(mesh, i + 0, scale);
            const Vector3 v1 = LoadScaledVertex(mesh, i + 1, scale);
            const Vector3 v2 = LoadScaledVertex(mesh, i + 2, scale);
            const std::string key = MakeTriangleKey(v0, v1, v2);
            if (dedupSet.insert(key).second) {
                outTriangles.push_back({v0, v1, v2, MakeTriangleBounds(v0, v1, v2)});
            }
        }
    }
}

}

void CollisionMap::BuildFromModel(const Model& model, const Vector3& scale) {
    Clear();

    if (model.meshCount <= 0 || model.meshes == nullptr) {
        return;
    }

    std::unordered_set<std::string> dedupSet;
    dedupSet.reserve(static_cast<size_t>(model.meshCount) * 256);

    for (int i = 0; i < model.meshCount; ++i) {
        AddBoxesFromMesh(model.meshes[i], scale, triangles_, dedupSet);
    }
}

void CollisionMap::Clear() {
    triangles_.clear();
}

const std::vector<CollisionTriangle>& CollisionMap::GetTriangles() const {
    return triangles_;
}

bool CollisionMap::OverlapsAny(const Vector3& center, const Vector3& halfExtents) const {
    const float radius = std::max(0.05f, std::min(halfExtents.x, halfExtents.z));
    const float verticalOffset = std::max(0.0f, halfExtents.y - radius);

    const Vector3 sampleCenters[3] = {
        center,
        {center.x, center.y - verticalOffset, center.z},
        {center.x, center.y + verticalOffset, center.z},
    };

    for (const auto& tri : triangles_) {
        for (const Vector3& sampleCenter : sampleCenters) {
            if (SphereIntersectsTriangle(sampleCenter, radius, tri)) {
                return true;
            }
        }
    }
    return false;
}
