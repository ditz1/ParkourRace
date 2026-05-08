#include "player/Player.h"

#include <cmath>

#include "world/CollisionMap.h"

Player::Player()
    : position_{0.0f, 1.5f, 0.0f},
      moveSpeed_(6.0f),
      eyeHeight_(1.7f),
      bodyHeight_(1.8f),
      bodyHalfWidth_(0.3f),
      bodyHalfDepth_(0.3f) {}

void Player::Update(float deltaTime,
                    const Vector3& planarForward,
                    const Vector3& planarRight,
                    bool allowInput,
                    const CollisionMap& collisionMap) {
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

    const float stepX = moveDirection.x * moveSpeed_ * deltaTime;
    const float stepZ = moveDirection.z * moveSpeed_ * deltaTime;

    const Vector3 halfExtents = GetHalfExtents();

    if (stepX != 0.0f) {
        const Vector3 candidatePosition = {position_.x + stepX, position_.y, position_.z};
        const Vector3 candidateCenter = GetBodyCenter(candidatePosition);
        if (!collisionMap.OverlapsAny(candidateCenter, halfExtents)) {
            position_.x = candidatePosition.x;
        }
    }

    if (stepZ != 0.0f) {
        const Vector3 candidatePosition = {position_.x, position_.y, position_.z + stepZ};
        const Vector3 candidateCenter = GetBodyCenter(candidatePosition);
        if (!collisionMap.OverlapsAny(candidateCenter, halfExtents)) {
            position_.z = candidatePosition.z;
        }
    }
}

void Player::Draw3D() const {
    const Vector3 bodyCenter = GetBodyCenter(position_);
    const float fullWidth = bodyHalfWidth_ * 2.0f;
    const float fullDepth = bodyHalfDepth_ * 2.0f;
    DrawCube(bodyCenter, fullWidth, bodyHeight_, fullDepth, BLUE);
    DrawCubeWires(bodyCenter, fullWidth, bodyHeight_, fullDepth, DARKBLUE);
}

const Vector3& Player::GetPosition() const {
    return position_;
}

float Player::GetEyeHeight() const {
    return eyeHeight_;
}

Vector3 Player::GetHalfExtents() const {
    return {bodyHalfWidth_, bodyHeight_ * 0.5f, bodyHalfDepth_};
}

Vector3 Player::GetBodyCenter(const Vector3& position) const {
    return {position.x, position.y + bodyHeight_ * 0.5f, position.z};
}
