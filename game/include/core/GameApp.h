#pragma once

#include "camera/CameraController.h"
#include "player/Player.h"
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

    CameraController cameraController_;
    Player player_;
    GridWorld world_;
};
