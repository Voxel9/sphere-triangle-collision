#pragma once

#include "main.h"

class StaticMesh {
public:
    StaticMesh(const char *filepath);
    
    void Draw();
    
    std::vector<vec3> vertices;
    std::vector<vec2> uvs;
    std::vector<vec3> normals;
    
    std::vector<unsigned int> vertex_indices;
    std::vector<unsigned int> uv_indices;
    std::vector<unsigned int> normal_indices;
    
    unsigned int num_faces;
};
