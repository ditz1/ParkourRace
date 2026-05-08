#include "world/ObjMesh.h"

#include <fstream>
#include <sstream>
#include <string>

namespace {

int ParseFaceVertexIndex(const std::string& token, int currentVertexCount) {
    const size_t slashPos = token.find('/');
    const std::string indexPart = (slashPos == std::string::npos) ? token : token.substr(0, slashPos);

    if (indexPart.empty()) {
        return -1;
    }

    int parsed = 0;
    try {
        parsed = std::stoi(indexPart);
    } catch (...) {
        return -1;
    }

    if (parsed > 0) {
        return parsed - 1;
    }

    if (parsed < 0) {
        return currentVertexCount + parsed;
    }

    return -1;
}

}

bool ObjMesh::LoadFromFile(const std::string& path) {
    Clear();

    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.size() < 2) {
            continue;
        }

        if (line[0] == 'v' && line[1] == ' ') {
            std::istringstream stream(line.substr(2));
            float x = 0.0f, y = 0.0f, z = 0.0f;
            if (stream >> x >> y >> z) {
                vertices_.push_back({x, y, z});
            }
            continue;
        }

        if (line[0] == 'f' && line[1] == ' ') {
            std::istringstream stream(line.substr(2));
            std::string token;
            std::vector<int> face;
            while (stream >> token) {
                const int idx = ParseFaceVertexIndex(token, static_cast<int>(vertices_.size()));
                if (idx >= 0) {
                    face.push_back(idx);
                }
            }
            if (face.size() >= 3) {
                faces_.push_back(std::move(face));
            }
        }
    }

    return !vertices_.empty();
}

void ObjMesh::Clear() {
    vertices_.clear();
    faces_.clear();
}

const std::vector<Vector3>& ObjMesh::GetVertices() const {
    return vertices_;
}

const std::vector<std::vector<int>>& ObjMesh::GetFaces() const {
    return faces_;
}

bool ObjMesh::WriteTriangulated(const std::string& outPath) const {
    if (vertices_.empty() || faces_.empty()) {
        return false;
    }

    std::ofstream file(outPath);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& v : vertices_) {
        file << "v " << v.x << " " << v.y << " " << v.z << "\n";
    }

    for (const auto& face : faces_) {
        if (face.size() < 3) {
            continue;
        }
        for (size_t i = 1; i + 1 < face.size(); ++i) {
            file << "f " << (face[0] + 1) << " " << (face[i] + 1) << " " << (face[i + 1] + 1) << "\n";
        }
    }

    return file.good();
}
