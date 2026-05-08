#pragma once

#include "raylib.h"

class Player {
public:
    Player();

    void Update(float deltaTime, const Vector3& planarForward, const Vector3& planarRight, bool allowInput);
    void Draw3D() const;

    const Vector3& GetPosition() const;
    float GetEyeHeight() const;

private:
    Vector3 position_;
    float moveSpeed_;
    float eyeHeight_;
    float bodyHeight_;
};
