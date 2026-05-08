#include "core/GameApp.h"

#include "core/AppConfig.h"

GameApp::GameApp() : showDebugBoundingBoxes_(false) {}

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

    level_.Load(AppConfig::LevelModelPath);
}

void GameApp::Shutdown() {
    if (IsWindowReady()) {
        level_.Unload();
        EnableCursor();
        CloseWindow();
    }
}

void GameApp::Update() {
    const float deltaTime = GetFrameTime();

    if (IsKeyPressed(KEY_B)) {
        showDebugBoundingBoxes_ = !showDebugBoundingBoxes_;
    }

    cameraController_.Update(deltaTime, player_);
    player_.Update(deltaTime,
                   cameraController_.GetPlanarForward(),
                   cameraController_.GetPlanarRight(),
                   cameraController_.IsAttachedToPlayer(),
                   level_.GetCollisionMap());
    world_.Update(deltaTime);
}

void GameApp::Draw() const {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(cameraController_.GetCamera());
    world_.Draw3D();
    level_.Draw3D();
    if (!cameraController_.IsAttachedToPlayer()) {
        player_.Draw3D();
    }
    if (showDebugBoundingBoxes_) {
        level_.DrawDebugCollisionTriangles();
    }
    EndMode3D();

    const char* modeText = cameraController_.IsAttachedToPlayer()
        ? "Mode: attached player camera | C: free debug camera"
        : "Mode: free debug camera | C: reattach to player";
    const char* boundsText = showDebugBoundingBoxes_
        ? "Collision triangles: ON  | B: toggle"
        : "Collision triangles: OFF | B: toggle";
    DrawText(modeText, 16, 16, 20, DARKGRAY);
    DrawText("WASD movement works in attached mode", 16, 42, 20, GRAY);
    DrawText(boundsText, 16, 68, 20, DARKGREEN);
    DrawFPS(16, 96);

    EndDrawing();
}
