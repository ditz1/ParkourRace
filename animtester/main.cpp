#include "raylib.h"

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "parkourplayer animation tester");
    SetTargetFPS(60);
    DisableCursor();

    Camera3D camera = {
        {3.0f, 2.0f, 3.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        45.0f,
        CAMERA_PERSPECTIVE
    };

    const char* modelPath = "../assets/parkourplayer.glb";
    Model model = LoadModel(modelPath);
    BoundingBox bounds = GetModelBoundingBox(model);
    Vector3 center = {
        (bounds.min.x + bounds.max.x) * 0.5f,
        (bounds.min.y + bounds.max.y) * 0.5f,
        (bounds.min.z + bounds.max.z) * 0.5f
    };
    Vector3 drawPosition = {0.0f, 0.0f, 0.0f};
    BoundingBox centeredBounds = {
        {bounds.min.x - center.x, bounds.min.y - center.y, bounds.min.z - center.z},
        {bounds.max.x - center.x, bounds.max.y - center.y, bounds.max.z - center.z}
    };

    int animCount = 0;
    ModelAnimation* animations = LoadModelAnimations(modelPath, &animCount);
    int activeAnim = 0;
    int frameCounter = 0;

    while (!WindowShouldClose()) {
        if (animCount > 0) {
            if (IsKeyPressed(KEY_RIGHT)) {
                activeAnim = (activeAnim + 1) % animCount;
                frameCounter = 0;
                UpdateModelAnimation(model, animations[activeAnim], frameCounter);
            } else if (IsKeyPressed(KEY_LEFT)) {
                activeAnim = (activeAnim - 1 + animCount) % animCount;
                frameCounter = 0;
                UpdateModelAnimation(model, animations[activeAnim], frameCounter);
            }
        }

        if (animCount > 0 && IsModelAnimationValid(model, animations[activeAnim])) {
            const int frameCount = animations[activeAnim].frameCount;
            if (frameCount > 0) {
                frameCounter = (frameCounter + 1) % frameCount;
                UpdateModelAnimation(model, animations[activeAnim], frameCounter);
            }
        }

        UpdateCamera(&camera, CAMERA_FREE);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        DrawGrid(20, 1.0f);
        DrawModelEx(model, drawPosition, {1.0f, 0.0f, 0.0f}, 90.0f, {0.1f, 0.1f, 0.1f}, WHITE);
        DrawBoundingBox(centeredBounds, GREEN);
        EndMode3D();

        DrawRectangle(10, 10, 430, 150, Fade(SKYBLUE, 0.25f));
        DrawRectangleLines(10, 10, 430, 150, BLUE);
        DrawText("Free camera: WASD + mouse, Q/E up/down", 20, 20, 20, DARKBLUE);
        DrawText(TextFormat("Model: %s", modelPath), 20, 48, 20, DARKBLUE);
        DrawText(TextFormat("Animations found: %d", animCount), 20, 76, 20, DARKBLUE);
        DrawText(TextFormat("Active animation: %d (left/right to switch)", activeAnim), 20, 104, 20, DARKBLUE);
        DrawText("Esc to quit", 20, 132, 20, DARKBLUE);

        EndDrawing();
    }

    if (animations != nullptr) {
        UnloadModelAnimations(animations, animCount);
    }
    UnloadModel(model);
    CloseWindow();

    return 0;
}
