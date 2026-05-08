#include "player/Player.h"

#include <cmath>

#include "core/AppConfig.h"
#include "world/CollisionMap.h"

namespace {
constexpr float kAnimationFps = 30.0f;
constexpr float kYawLerpSpeed = 14.0f;
constexpr float kMinimumPlanarLengthSq = 0.000001f;
}

Player::Player()
    : position_{0.0f, 1.5f, 0.0f},
      velocity_{0.0f, 0.0f, 0.0f},
      facingDirection_{0.0f, 0.0f, 1.0f},
      moveSpeed_(6.0f),
      slideMoveSpeed_(7.0f),
      jumpVelocity_(6.0f),
      gravity_(-16.0f),
      groundY_(1.5f),
      eyeHeight_(1.7f),
      standingEyeHeight_(1.7f),
      slidingEyeHeight_(1.15f),
      bodyHeight_(1.8f),
      standingBodyHeight_(1.8f),
      slidingBodyHeight_(1.1f),
      bodyHalfWidth_(0.3f),
      bodyHalfDepth_(0.3f),
      modelScale_(AppConfig::PlayerModelScale),
      model_{},
      modelLoaded_(false),
      animations_(nullptr),
      animationCount_(0),
      animationState_(AnimState::Idle),
      currentAnimationFrame_(0),
      animationFrameAccumulator_(0.0f),
      animationReversePlayback_(false),
      slideTimer_(0.0f),
      slideDuration_(0.55f),
      jumpRequested_(false),
      isGrounded_(true) {}

Player::~Player() {
    Unload();
}

bool Player::Load() {
    Unload();

    model_ = LoadModel(AppConfig::PlayerModelPath);
    modelLoaded_ = (model_.meshCount > 0);
    if (!modelLoaded_) {
        return false;
    }

    animations_ = LoadModelAnimations(AppConfig::PlayerModelPath, &animationCount_);
    if (animations_ == nullptr || animationCount_ <= 0) {
        if (animations_ != nullptr) {
            UnloadModelAnimations(animations_, animationCount_);
        }
        animations_ = nullptr;
        animationCount_ = 0;
    }

    SetAnimationState(AnimState::Idle);
    return true;
}

void Player::Unload() {
    if (animations_ != nullptr) {
        UnloadModelAnimations(animations_, animationCount_);
        animations_ = nullptr;
        animationCount_ = 0;
    }

    if (modelLoaded_) {
        UnloadModel(model_);
        modelLoaded_ = false;
    }
}

void Player::Update(float deltaTime,
                    const Vector3& planarForward,
                    const Vector3& planarRight,
                    bool allowInput,
                    const CollisionMap& collisionMap) {
    if (allowInput && IsKeyPressed(KEY_SPACE)) {
        jumpRequested_ = true;
    }

    if (allowInput && IsKeyPressed(KEY_F) && isGrounded_ && slideTimer_ <= 0.0f) {
        slideTimer_ = slideDuration_;
    }

    ApplyMovementAndCollision(deltaTime, planarForward, planarRight, allowInput, collisionMap);
    AdvanceAnimation(deltaTime);
}

void Player::Draw3D() const {
    if (!modelLoaded_) {
        const Vector3 bodyCenter = GetBodyCenter(position_);
        const float fullWidth = bodyHalfWidth_ * 2.0f;
        const float fullDepth = bodyHalfDepth_ * 2.0f;
        DrawCube(bodyCenter, fullWidth, bodyHeight_, fullDepth, BLUE);
        DrawCubeWires(bodyCenter, fullWidth, bodyHeight_, fullDepth, DARKBLUE);
        return;
    }

    DrawModelEx(model_,
                position_,
                Vector3{1.0f, 0.0f, 0.0f},
                90.0f,
                Vector3{modelScale_, modelScale_, modelScale_},
                WHITE);
}

const Vector3& Player::GetPosition() const {
    return position_;
}

float Player::GetEyeHeight() const {
    return eyeHeight_;
}

Vector3 Player::GetFacingPlanarDirection() const {
    return facingDirection_;
}

bool Player::IsFirstPersonBodyHidden() const {
    return true;
}

bool Player::IsModelLoaded() const {
    return modelLoaded_;
}

int Player::ResolveAnimationIndex(int preferredIndex) const {
    if (animationCount_ <= 0) {
        return -1;
    }
    if (preferredIndex >= 0 && preferredIndex < animationCount_) {
        return preferredIndex;
    }
    return 0;
}

void Player::AdvanceAnimation(float deltaTime) {
    if (!modelLoaded_ || animations_ == nullptr || animationCount_ <= 0) {
        return;
    }

    int activeAnimationIndex = ResolveAnimationIndex(kIdleAnimationIndex);
    switch (animationState_) {
        case AnimState::Idle:
            activeAnimationIndex = ResolveAnimationIndex(kIdleAnimationIndex);
            break;
        case AnimState::RunForward:
            activeAnimationIndex = ResolveAnimationIndex(kRunAnimationIndex);
            break;
        case AnimState::RunBackward:
            activeAnimationIndex = ResolveAnimationIndex(kRunAnimationIndex);
            break;
        case AnimState::Slide:
            activeAnimationIndex = ResolveAnimationIndex(kSlideAnimationIndex);
            break;
        case AnimState::Jump:
            activeAnimationIndex = ResolveAnimationIndex(kJumpAnimationIndex);
            break;
    }

    if (activeAnimationIndex < 0) {
        return;
    }
    if (!IsModelAnimationValid(model_, animations_[activeAnimationIndex])) {
        return;
    }

    const int frameCount = animations_[activeAnimationIndex].frameCount;
    if (frameCount <= 0) {
        return;
    }

    animationFrameAccumulator_ += deltaTime * kAnimationFps;
    while (animationFrameAccumulator_ >= 1.0f) {
        animationFrameAccumulator_ -= 1.0f;
        if (animationReversePlayback_) {
            currentAnimationFrame_ = (currentAnimationFrame_ - 1 + frameCount) % frameCount;
        } else {
            currentAnimationFrame_ = (currentAnimationFrame_ + 1) % frameCount;
        }
    }

    if (currentAnimationFrame_ >= frameCount) {
        currentAnimationFrame_ = frameCount - 1;
    }
    if (currentAnimationFrame_ < 0) {
        currentAnimationFrame_ = 0;
    }
    UpdateModelAnimation(model_, animations_[activeAnimationIndex], currentAnimationFrame_);
}

void Player::ApplyMovementAndCollision(float deltaTime,
                                       const Vector3& planarForward,
                                       const Vector3& planarRight,
                                       bool allowInput,
                                       const CollisionMap& collisionMap) {
    Vector2 inputAxis = {0.0f, 0.0f};
    if (allowInput) {
        if (IsKeyDown(KEY_W)) inputAxis.y += 1.0f;
        if (IsKeyDown(KEY_S)) inputAxis.y -= 1.0f;
        if (IsKeyDown(KEY_D)) inputAxis.x += 1.0f;
        if (IsKeyDown(KEY_A)) inputAxis.x -= 1.0f;
    }

    const bool hasInput = (inputAxis.x != 0.0f) || (inputAxis.y != 0.0f);
    if (hasInput) {
        const float axisLength = std::sqrt((inputAxis.x * inputAxis.x) + (inputAxis.y * inputAxis.y));
        inputAxis.x /= axisLength;
        inputAxis.y /= axisLength;
    }

    const Vector3 requestedMoveDirection = {
        (planarRight.x * inputAxis.x) + (planarForward.x * inputAxis.y),
        0.0f,
        (planarRight.z * inputAxis.x) + (planarForward.z * inputAxis.y),
    };

    const Vector3 normalizedMoveDirection = NormalizePlanar(requestedMoveDirection);

    if (slideTimer_ > 0.0f) {
        slideTimer_ -= deltaTime;
        if (slideTimer_ <= 0.0f) {
            slideTimer_ = 0.0f;
        }
    }

    const bool isSliding = (slideTimer_ > 0.0f);
    bodyHeight_ = isSliding ? slidingBodyHeight_ : standingBodyHeight_;
    eyeHeight_ = isSliding ? slidingEyeHeight_ : standingEyeHeight_;

    if (jumpRequested_ && isGrounded_ && !isSliding) {
        velocity_.y = jumpVelocity_;
        isGrounded_ = false;
    }
    jumpRequested_ = false;

    velocity_.y += gravity_ * deltaTime;
    const float speed = isSliding ? slideMoveSpeed_ : moveSpeed_;
    const float stepX = normalizedMoveDirection.x * speed * deltaTime;
    const float stepZ = normalizedMoveDirection.z * speed * deltaTime;

    const Vector3 halfExtents = GetHalfExtents();
    Vector3 updatedPosition = position_;

    if (stepX != 0.0f) {
        const Vector3 candidatePosition = {updatedPosition.x + stepX, updatedPosition.y, updatedPosition.z};
        const Vector3 candidateCenter = GetBodyCenter(candidatePosition);
        if (!collisionMap.OverlapsAny(candidateCenter, halfExtents)) {
            updatedPosition.x = candidatePosition.x;
        }
    }

    if (stepZ != 0.0f) {
        const Vector3 candidatePosition = {updatedPosition.x, updatedPosition.y, updatedPosition.z + stepZ};
        const Vector3 candidateCenter = GetBodyCenter(candidatePosition);
        if (!collisionMap.OverlapsAny(candidateCenter, halfExtents)) {
            updatedPosition.z = candidatePosition.z;
        }
    }

    const float stepY = velocity_.y * deltaTime;
    if (stepY != 0.0f) {
        const Vector3 candidatePosition = {updatedPosition.x, updatedPosition.y + stepY, updatedPosition.z};
        const Vector3 candidateCenter = GetBodyCenter(candidatePosition);
        if (!collisionMap.OverlapsAny(candidateCenter, halfExtents)) {
            updatedPosition.y = candidatePosition.y;
        } else if (velocity_.y < 0.0f) {
            velocity_.y = 0.0f;
        }
    }

    if (updatedPosition.y <= groundY_) {
        updatedPosition.y = groundY_;
        velocity_.y = 0.0f;
        isGrounded_ = true;
    } else {
        isGrounded_ = false;
    }

    position_ = updatedPosition;

    if (hasInput && !isSliding) {
        const Vector3 targetFacing = normalizedMoveDirection;
        const float blend = std::fmin(1.0f, deltaTime * kYawLerpSpeed);
        facingDirection_.x += (targetFacing.x - facingDirection_.x) * blend;
        facingDirection_.z += (targetFacing.z - facingDirection_.z) * blend;
        facingDirection_ = NormalizePlanar(facingDirection_);
    }

    const float forwardDot = (normalizedMoveDirection.x * planarForward.x) + (normalizedMoveDirection.z * planarForward.z);
    const bool moving = hasInput && (std::fabs(normalizedMoveDirection.x) > 0.0f || std::fabs(normalizedMoveDirection.z) > 0.0f);

    if (!isGrounded_) {
        SetAnimationState(AnimState::Jump);
    } else if (isSliding) {
        SetAnimationState(AnimState::Slide);
    } else if (moving) {
        if (forwardDot >= 0.0f) {
            SetAnimationState(AnimState::RunForward);
        } else {
            SetAnimationState(AnimState::RunBackward);
        }
    } else {
        SetAnimationState(AnimState::Idle);
    }
}

Vector3 Player::GetHalfExtents() const {
    return {bodyHalfWidth_, bodyHeight_ * 0.5f, bodyHalfDepth_};
}

Vector3 Player::GetBodyCenter(const Vector3& position) const {
    return {position.x, position.y + bodyHeight_ * 0.5f, position.z};
}

void Player::SetAnimationState(AnimState state) {
    if (animationState_ != state) {
        animationState_ = state;
        animationFrameAccumulator_ = 0.0f;
        currentAnimationFrame_ = 0;
    }

    animationReversePlayback_ = (state == AnimState::RunBackward);
}

Vector3 Player::NormalizePlanar(Vector3 v) const {
    v.y = 0.0f;
    const float lenSq = (v.x * v.x) + (v.z * v.z);
    if (lenSq < kMinimumPlanarLengthSq) {
        return {0.0f, 0.0f, 1.0f};
    }

    const float invLen = 1.0f / std::sqrt(lenSq);
    return {v.x * invLen, 0.0f, v.z * invLen};
}
