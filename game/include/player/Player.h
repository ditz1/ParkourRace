#pragma once

#include "raylib.h"

class CollisionMap;

class Player {
public:
    Player();
    ~Player();

    bool Load();
    void Unload();

    void Update(float deltaTime,
                const Vector3& planarForward,
                const Vector3& planarRight,
                bool allowInput,
                const CollisionMap& collisionMap);
    void Draw3D() const;

    const Vector3& GetPosition() const;
    float GetEyeHeight() const;
    Vector3 GetFacingPlanarDirection() const;
    bool IsFirstPersonBodyHidden() const;
    bool IsModelLoaded() const;

private:
    enum class AnimState {
        Idle,
        RunForward,
        RunBackward,
        Slide,
        Jump,
    };

    static constexpr int kIdleAnimationIndex = 5;
    static constexpr int kJumpAnimationIndex = 6;
    static constexpr int kRunAnimationIndex = 8;
    static constexpr int kSlideAnimationIndex = 9;

    int ResolveAnimationIndex(int preferredIndex) const;
    void AdvanceAnimation(float deltaTime);
    void ApplyMovementAndCollision(float deltaTime,
                                   const Vector3& planarForward,
                                   const Vector3& planarRight,
                                   bool allowInput,
                                   const CollisionMap& collisionMap);
    Vector3 GetHalfExtents() const;
    Vector3 GetBodyCenter(const Vector3& position) const;
    void SetAnimationState(AnimState state);
    Vector3 NormalizePlanar(Vector3 v) const;

    Vector3 position_;
    Vector3 velocity_;
    Vector3 facingDirection_;
    float moveSpeed_;
    float slideMoveSpeed_;
    float jumpVelocity_;
    float gravity_;
    float groundY_;
    float eyeHeight_;
    float standingEyeHeight_;
    float slidingEyeHeight_;
    float bodyHeight_;
    float standingBodyHeight_;
    float slidingBodyHeight_;
    float bodyHalfWidth_;
    float bodyHalfDepth_;
    float modelScale_;
    float modelYawDegrees_;

    Model model_;
    bool modelLoaded_;
    ModelAnimation* animations_;
    int animationCount_;
    AnimState animationState_;
    int currentAnimationFrame_;
    float animationFrameAccumulator_;
    bool animationReversePlayback_;
    float slideTimer_;
    float slideDuration_;
    bool jumpRequested_;
    bool isGrounded_;
};
