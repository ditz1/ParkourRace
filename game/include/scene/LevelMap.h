#pragma once

#include <string>

#include "raylib.h"
#include "world/CollisionMap.h"

class LevelMap {
public:
    LevelMap();
    ~LevelMap();

    LevelMap(const LevelMap&) = delete;
    LevelMap& operator=(const LevelMap&) = delete;

    bool Load(const std::string& modelPath);
    void Unload();

    void Draw3D() const;
    void DrawDebugCollisionTriangles() const;

    const CollisionMap& GetCollisionMap() const;

private:
    void SetupMaterialAndShader();

    Model model_;
    bool modelLoaded_;
    Shader celShader_;
    bool shaderLoaded_;
    Vector3 scale_;
    CollisionMap collisionMap_;
};
