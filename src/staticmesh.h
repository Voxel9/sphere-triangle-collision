#pragma once

#include "main.h"
#include "texture.h"

typedef struct {
    char name[128];
    
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
    
    Texture *texture;
    GLuint gl_tex_id;
} static_mesh_material;

typedef struct {
    unsigned int material_index;
    
    std::vector<unsigned int> vertex_indices;
    std::vector<unsigned int> uv_indices;
    std::vector<unsigned int> normal_indices;
    
    unsigned int num_faces;
} static_mesh_submesh;

typedef struct {
    char name[128];
    
    std::vector<static_mesh_submesh> submeshes;
} static_mesh_group;

class StaticMesh {
public:
    StaticMesh(const char *directory, const char *filename);
    ~StaticMesh();
    
    void Draw();
    
    std::vector<vec3> vertices;
    std::vector<vec2> uvs;
    std::vector<vec3> normals;
    
    std::vector<static_mesh_material> materials;
    std::vector<static_mesh_group> groups;
};
