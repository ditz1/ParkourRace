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
    player_.Load();
}

void GameApp::Shutdown() {
    if (IsWindowReady()) {
        player_.Unload();
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
    if (!cameraController_.IsFirstPerson() || !player_.IsFirstPersonBodyHidden()) {
        player_.Draw3D();
    }
    if (showDebugBoundingBoxes_) {
        level_.DrawDebugCollisionTriangles();
    }
    EndMode3D();

    const char* modeText = "";
    if (cameraController_.IsFirstPerson()) {
        modeText = "Mode: first person | C: third person";
    } else if (cameraController_.IsThirdPerson()) {
        modeText = "Mode: third person | C: free camera";
    } else {
        modeText = "Mode: free camera | C: first person";
    }
    const char* boundsText = showDebugBoundingBoxes_
        ? "Collision triangles: ON  | B: toggle"
        : "Collision triangles: OFF | B: toggle";
    DrawText(modeText, 16, 16, 20, DARKGRAY);
    DrawText("WASD move | Space jump | F slide", 16, 42, 20, GRAY);
    DrawText(boundsText, 16, 68, 20, DARKGREEN);
    DrawFPS(16, 96);

    EndDrawing();
}
