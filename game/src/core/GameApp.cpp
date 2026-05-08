#include "core/GameApp.h"

#include "core/AppConfig.h"

GameApp::GameApp()
    : camera_{
          .position = Vector3{10.0f, 10.0f, 10.0f},
          .target = Vector3{0.0f, 0.0f, 0.0f},
          .up = Vector3{0.0f, 1.0f, 0.0f},
          .fovy = 45.0f,
          .projection = CAMERA_PERSPECTIVE,
      } {}

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
}

void GameApp::Shutdown() {
    if (IsWindowReady()) {
        CloseWindow();
    }
}

void GameApp::Update() {
    UpdateCamera(&camera_, CAMERA_FREE);
    world_.Update(GetFrameTime());
}

void GameApp::Draw() const {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(camera_);
    world_.Draw3D();
    EndMode3D();

    DrawText("3D world prototype: WASD + mouse (free camera)", 16, 16, 20, DARKGRAY);
    DrawFPS(16, 44);

    EndDrawing();
}
