#include "camera/CameraController.h"

#include <cmath>

namespace {
constexpr float kPitchLimitRadians = 1.45f;
}

CameraController::CameraController()
    : camera_{
          .position = Vector3{0.0f, 1.7f, 0.0f},
          .target = Vector3{0.0f, 1.7f, 1.0f},
          .up = Vector3{0.0f, 1.0f, 0.0f},
          .fovy = 60.0f,
          .projection = CAMERA_PERSPECTIVE,
      },
      mode_(CameraControlMode::FirstPerson),
      yawRadians_(0.0f),
      pitchRadians_(0.0f),
      mouseSensitivity_(0.0025f),
      thirdPersonDistance_(5.0f),
      thirdPersonHeightOffset_(1.2f) {}

void CameraController::Update(float deltaTime, const Player& player) {
    (void)deltaTime;

    if (IsKeyPressed(KEY_C)) {
        ToggleMode(player);
    }

    if (mode_ == CameraControlMode::FirstPerson || mode_ == CameraControlMode::ThirdPerson) {
        UpdateFollowPlayer(player);
        return;
    }

    UpdateCamera(&camera_, CAMERA_FREE);
}

const Camera3D& CameraController::GetCamera() const {
    return camera_;
}

Vector3 CameraController::GetPlanarForward() const {
    Vector3 forward = ComputeForwardDirection();
    forward.y = 0.0f;

    const float planarLenSq = (forward.x * forward.x) + (forward.z * forward.z);
    if (planarLenSq <= 0.000001f) {
        return {0.0f, 0.0f, 1.0f};
    }

    const float invLen = 1.0f / std::sqrt(planarLenSq);
    return {forward.x * invLen, 0.0f, forward.z * invLen};
}

Vector3 CameraController::GetPlanarRight() const {
    const Vector3 forward = GetPlanarForward();
    return {-forward.z, 0.0f, forward.x};
}

bool CameraController::IsAttachedToPlayer() const {
    return mode_ != CameraControlMode::FreeDebug;
}

bool CameraController::IsFirstPerson() const {
    return mode_ == CameraControlMode::FirstPerson;
}

bool CameraController::IsThirdPerson() const {
    return mode_ == CameraControlMode::ThirdPerson;
}

CameraControlMode CameraController::GetMode() const {
    return mode_;
}

void CameraController::ToggleMode(const Player& player) {
    if (mode_ == CameraControlMode::FirstPerson) {
        mode_ = CameraControlMode::ThirdPerson;
        SyncCameraToPlayer(player);
        return;
    }

    if (mode_ == CameraControlMode::ThirdPerson) {
        mode_ = CameraControlMode::FreeDebug;
        return;
    }

    mode_ = CameraControlMode::FirstPerson;
    SyncCameraToPlayer(player);
}

void CameraController::UpdateFollowPlayer(const Player& player) {
    const Vector2 mouseDelta = GetMouseDelta();
    yawRadians_ -= mouseDelta.x * mouseSensitivity_;
    pitchRadians_ -= mouseDelta.y * mouseSensitivity_;

    if (pitchRadians_ > kPitchLimitRadians) pitchRadians_ = kPitchLimitRadians;
    if (pitchRadians_ < -kPitchLimitRadians) pitchRadians_ = -kPitchLimitRadians;

    SyncCameraToPlayer(player);
}

void CameraController::SyncCameraToPlayer(const Player& player) {
    const Vector3 playerPos = player.GetPosition();
    const Vector3 forward = ComputeForwardDirection();

    if (mode_ == CameraControlMode::ThirdPerson) {
        const Vector3 focusPoint = {playerPos.x, playerPos.y + player.GetEyeHeight() * 0.8f, playerPos.z};
        camera_.position = {
            focusPoint.x - (forward.x * thirdPersonDistance_),
            focusPoint.y + thirdPersonHeightOffset_,
            focusPoint.z - (forward.z * thirdPersonDistance_),
        };
        camera_.target = focusPoint;
        return;
    }

    camera_.position = {playerPos.x, playerPos.y + player.GetEyeHeight(), playerPos.z};
    camera_.target = {
        camera_.position.x + forward.x,
        camera_.position.y + forward.y,
        camera_.position.z + forward.z,
    };
}

Vector3 CameraController::ComputeForwardDirection() const {
    const float cosPitch = std::cosf(pitchRadians_);
    const float sinPitch = std::sinf(pitchRadians_);
    const float sinYaw = std::sinf(yawRadians_);
    const float cosYaw = std::cosf(yawRadians_);

    return {
        sinYaw * cosPitch,
        sinPitch,
        cosYaw * cosPitch,
    };
}
