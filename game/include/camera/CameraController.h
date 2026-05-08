#pragma once

#include "player/Player.h"
#include "raylib.h"

enum class CameraControlMode {
    AttachedToPlayer,
    FreeDebug,
};

class CameraController {
public:
    CameraController();

    void Update(float deltaTime, const Player& player);
    const Camera3D& GetCamera() const;

    Vector3 GetPlanarForward() const;
    Vector3 GetPlanarRight() const;
    bool IsAttachedToPlayer() const;
    CameraControlMode GetMode() const;

private:
    void ToggleMode(const Player& player);
    void UpdateAttachedToPlayer(const Player& player);
    void SyncCameraToPlayer(const Player& player);
    Vector3 ComputeForwardDirection() const;

    Camera3D camera_;
    CameraControlMode mode_;
    float yawRadians_;
    float pitchRadians_;
    float mouseSensitivity_;
};
