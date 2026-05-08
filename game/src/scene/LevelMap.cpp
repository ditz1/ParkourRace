#include "scene/LevelMap.h"

#include "core/AppConfig.h"

namespace {
const Vector3 kLightDir = {-0.4f, -1.0f, -0.3f};
const float kAmbientStrength = 0.22f;

Matrix MakeScaleMatrix(const Vector3& scale) {
    Matrix m = {
        scale.x, 0.0f,   0.0f,   0.0f,
        0.0f,   scale.y, 0.0f,   0.0f,
        0.0f,   0.0f,   scale.z, 0.0f,
        0.0f,   0.0f,   0.0f,   1.0f,
    };
    return m;
}
}

LevelMap::LevelMap()
    : model_{},
      modelLoaded_(false),
      celShader_{},
      shaderLoaded_(false),
      scale_{AppConfig::LevelScale, AppConfig::LevelScale, AppConfig::LevelScale} {}

LevelMap::~LevelMap() {
    Unload();
}

bool LevelMap::Load(const std::string& modelPath) {
    Unload();

    model_ = LoadModel(modelPath.c_str());
    modelLoaded_ = (model_.meshCount > 0);
    if (!modelLoaded_) {
        return false;
    }

    model_.transform = MakeScaleMatrix(scale_);
    collisionMap_.BuildFromModel(model_, scale_);
    SetupMaterialAndShader();

    return modelLoaded_;
}

void LevelMap::Unload() {
    if (shaderLoaded_) {
        UnloadShader(celShader_);
        shaderLoaded_ = false;
    }

    if (modelLoaded_) {
        UnloadModel(model_);
        modelLoaded_ = false;
    }
    collisionMap_.Clear();
}

void LevelMap::Draw3D() const {
    if (!modelLoaded_) {
        return;
    }
    DrawModel(model_, Vector3{0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
}

void LevelMap::DrawDebugCollisionTriangles() const {
    for (const auto& tri : collisionMap_.GetTriangles()) {
        DrawLine3D(tri.a, tri.b, GREEN);
        DrawLine3D(tri.b, tri.c, GREEN);
        DrawLine3D(tri.c, tri.a, GREEN);
    }
}

const CollisionMap& LevelMap::GetCollisionMap() const {
    return collisionMap_;
}

void LevelMap::SetupMaterialAndShader() {
    if (!modelLoaded_) {
        return;
    }

    celShader_ = LoadShader(AppConfig::CelVertexShaderPath, AppConfig::CelFragmentShaderPath);
    shaderLoaded_ = (celShader_.id > 0);

    for (int i = 0; i < model_.materialCount; ++i) {
        model_.materials[i].maps[MATERIAL_MAP_ALBEDO].color = BLUE;
        if (shaderLoaded_) {
            model_.materials[i].shader = celShader_;
        }
    }

    if (!shaderLoaded_) {
        return;
    }

    const int lightDirLoc = GetShaderLocation(celShader_, "lightDir");
    const int ambientLoc = GetShaderLocation(celShader_, "ambientStrength");

    SetShaderValue(celShader_, lightDirLoc, &kLightDir, SHADER_UNIFORM_VEC3);
    SetShaderValue(celShader_, ambientLoc, &kAmbientStrength, SHADER_UNIFORM_FLOAT);
}
