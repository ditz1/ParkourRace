#pragma once

struct AppConfig {
    static constexpr int ScreenWidth = 1280;
    static constexpr int ScreenHeight = 720;
    static constexpr const char* WindowTitle = "Parkour Race - 3D Prototype";
    static constexpr int TargetFps = 60;
    static constexpr const char* LevelModelPath = "assets/colltesting.glb";
    static constexpr const char* PlayerModelPath = "assets/parkourplayer.glb";
    static constexpr const char* CelVertexShaderPath = "shaders/cel_lit.vs";
    static constexpr const char* CelFragmentShaderPath = "shaders/cel_lit.fs";
    static constexpr float LevelScale = 10.0f;
    static constexpr float PlayerModelScale = 0.1f;
};
