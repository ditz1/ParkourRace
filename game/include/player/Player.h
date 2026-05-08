#pragma once

#include "raylib.h"

class CollisionMap;

class Player {
public:
    Player();

    void Update(float deltaTime,
                const Vector3& planarForward,
                const Vector3& planarRight,
                bool allowInput,
                const CollisionMap& collisionMap);
    void Draw3D() const;

    const Vector3& GetPosition() const;
    float GetEyeHeight() const;

private:
    Vector3 GetHalfExtents() const;
    Vector3 GetBodyCenter(const Vector3& position) const;

    Vector3 position_;
    float moveSpeed_;
    float eyeHeight_;
    float bodyHeight_;
    float bodyHalfWidth_;
    float bodyHalfDepth_;
};
