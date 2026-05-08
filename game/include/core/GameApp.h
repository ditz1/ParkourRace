#pragma once

#include "raylib.h"
#include "scene/GridWorld.h"

class GameApp {
public:
    GameApp();
    ~GameApp();

    void Run();

private:
    void Initialize();
    void Shutdown();
    void Update();
    void Draw() const;

    Camera3D camera_;
    GridWorld world_;
};
