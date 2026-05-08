#include "player/Player.h"

#include <cmath>

Player::Player()
    : position_{0.0f, 0.0f, 0.0f},
      moveSpeed_(6.0f),
      eyeHeight_(1.7f),
      bodyHeight_(1.8f) {}

void Player::Update(float deltaTime, const Vector3& planarForward, const Vector3& planarRight, bool allowInput) {
    if (!allowInput) {
        return;
    }

    Vector2 inputAxis = {0.0f, 0.0f};
    if (IsKeyDown(KEY_W)) inputAxis.y += 1.0f;
    if (IsKeyDown(KEY_S)) inputAxis.y -= 1.0f;
    if (IsKeyDown(KEY_D)) inputAxis.x += 1.0f;
    if (IsKeyDown(KEY_A)) inputAxis.x -= 1.0f;

    if (inputAxis.x == 0.0f && inputAxis.y == 0.0f) {
        return;
    }

    const float axisLength = std::sqrt((inputAxis.x * inputAxis.x) + (inputAxis.y * inputAxis.y));
    inputAxis.x /= axisLength;
    inputAxis.y /= axisLength;

    const Vector3 moveDirection = {
        (planarRight.x * inputAxis.x) + (planarForward.x * inputAxis.y),
        0.0f,
        (planarRight.z * inputAxis.x) + (planarForward.z * inputAxis.y),
    };

    position_.x += moveDirection.x * moveSpeed_ * deltaTime;
    position_.z += moveDirection.z * moveSpeed_ * deltaTime;
}

void Player::Draw3D() const {
    const Vector3 bodyCenter = {position_.x, bodyHeight_ * 0.5f, position_.z};
    DrawCube(bodyCenter, 0.6f, bodyHeight_, 0.6f, BLUE);
    DrawCubeWires(bodyCenter, 0.6f, bodyHeight_, 0.6f, DARKBLUE);
}

const Vector3& Player::GetPosition() const {
    return position_;
}

float Player::GetEyeHeight() const {
    return eyeHeight_;
}
