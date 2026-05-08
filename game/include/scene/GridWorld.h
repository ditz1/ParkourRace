#pragma once

#include "raylib.h"

class GridWorld {
public:
    GridWorld();

    void Update(float deltaTime);
    void Draw3D() const;

private:
    int gridSlices_;
    float gridSpacing_;
};
