#include "core/GameApp.h"

#include "core/AppConfig.h"

GameApp::GameApp() = default;

GameApp::~GameApp() {
    Shutdown();
}

void GameApp::Run() {
    Initialize();

    while (!WindowShouldClose()) {
        Update();
        Draw();
    }
}

void GameApp::Initialize() {
    InitWindow(AppConfig::ScreenWidth, AppConfig::ScreenHeight, AppConfig::WindowTitle);
    SetTargetFPS(AppConfig::TargetFps);
    DisableCursor();
}

void GameApp::Shutdown() {
    if (IsWindowReady()) {
        EnableCursor();
        CloseWindow();
    }
}

void GameApp::Update() {
    const float deltaTime = GetFrameTime();

    cameraController_.Update(deltaTime, player_);
    player_.Update(deltaTime,
                   cameraController_.GetPlanarForward(),
                   cameraController_.GetPlanarRight(),
                   cameraController_.IsAttachedToPlayer());
    world_.Update(deltaTime);
}

void GameApp::Draw() const {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(cameraController_.GetCamera());
    world_.Draw3D();
    player_.Draw3D();
    EndMode3D();

    const char* modeText = cameraController_.IsAttachedToPlayer()
        ? "Mode: attached player camera | C: free debug camera"
        : "Mode: free debug camera | C: reattach to player";
    DrawText(modeText, 16, 16, 20, DARKGRAY);
    DrawText("WASD movement works in attached mode", 16, 42, 20, GRAY);
    DrawFPS(16, 70);

    EndDrawing();
}
