#pragma once

#include <string>
#include <vector>

#include "raylib.h"

class ObjMesh {
public:
    bool LoadFromFile(const std::string& path);
    void Clear();

    const std::vector<Vector3>& GetVertices() const;
    const std::vector<std::vector<int>>& GetFaces() const;

    bool WriteTriangulated(const std::string& outPath) const;

private:
    std::vector<Vector3> vertices_;
    std::vector<std::vector<int>> faces_;
};
