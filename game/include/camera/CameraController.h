#pragma once

#include "player/Player.h"
#include "raylib.h"

enum class CameraControlMode {
    FirstPerson,
    ThirdPerson,
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
    bool IsFirstPerson() const;
    bool IsThirdPerson() const;
    CameraControlMode GetMode() const;

private:
    void ToggleMode(const Player& player);
    void UpdateFollowPlayer(const Player& player);
    void SyncCameraToPlayer(const Player& player);
    Vector3 ComputeForwardDirection() const;

    Camera3D camera_;
    CameraControlMode mode_;
    float yawRadians_;
    float pitchRadians_;
    float mouseSensitivity_;
    float thirdPersonDistance_;
    float thirdPersonHeightOffset_;
};
