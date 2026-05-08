#include "scene/GridWorld.h"

GridWorld::GridWorld() : gridSlices_(100), gridSpacing_(1.0f) {}

void GridWorld::Update(float deltaTime) {
    (void)deltaTime;
}

void GridWorld::Draw3D() const {
    DrawGrid(gridSlices_, gridSpacing_);
}
