#include "staticmesh.h"

// -----------------------------------------------------------------------------------
// Name: StaticMesh
// Desc: Constructor for the StaticMesh class.
//       Employs a very basic OBJ model file importer to load a mesh
// -----------------------------------------------------------------------------------
StaticMesh::StaticMesh(const char *directory, const char *filename) {
    char obj_filepath[256];
    strcpy(obj_filepath, directory);
    strcat(obj_filepath, filename);
    
    FILE *obj_file = fopen(obj_filepath, "r");
    
    if(!obj_file) {
        printf("Could not open OBJ file:\n%s\n", obj_filepath);
        return;
    }
    
    char linebuf[128];
    static_mesh_material *current_material = nullptr;
    static_mesh_submesh *current_submesh = nullptr;
    static_mesh_group *current_group = nullptr;
    
    while(fgets(linebuf, sizeof(linebuf), obj_file) != nullptr) {
        char prefixbuf[32];
        sscanf(linebuf, "%s", prefixbuf);
        
        // Parse MTL file
        if(!strcmp(prefixbuf, "mtllib")) {
            char mtl_filepath[256];
            char mtl_filename[128];
            
            sscanf(linebuf, "%s %s", prefixbuf, mtl_filename);
            
            strcpy(mtl_filepath, directory);
            strcat(mtl_filepath, mtl_filename);
            
            FILE *mtl_file = fopen(mtl_filepath, "r");
            
            if(!mtl_file) {
                printf("Could not open MTL file:\n%s\n", mtl_filepath);
                fclose(obj_file);
                return;
            }
            
            while(fgets(linebuf, sizeof(linebuf), mtl_file) != nullptr) {
                sscanf(linebuf, "%s", prefixbuf);
                
                // Parse new material
                if(!strcmp(prefixbuf, "newmtl")) {
                    static_mesh_material new_material;
                    new_material.diffuse  = vec4(1, 1, 1, 1);
                    new_material.ambient  = vec4(0, 0, 0, 1);
                    new_material.specular = vec4(0, 0, 0, 1);
                    new_material.texture  = nullptr;
                    
                    sscanf(linebuf, "%s %s", prefixbuf, new_material.name);
                    
                    materials.push_back(new_material);
                    current_material = &materials[materials.size() - 1];
                }
                
                // Parse diffuse color
                else if(!strcmp(prefixbuf, "Kd")) {
                    sscanf(linebuf, "%s %f %f %f", prefixbuf,
                        &current_material->diffuse.x,
                        &current_material->diffuse.y,
                        &current_material->diffuse.z
                        );
                }
                
                // Parse ambient color
                else if(!strcmp(prefixbuf, "Ka")) {
                    sscanf(linebuf, "%s %f %f %f", prefixbuf,
                        &current_material->ambient.x,
                        &current_material->ambient.y,
                        &current_material->ambient.z
                        );
                }
                
                // Parse transmission filter
                else if(!strcmp(prefixbuf, "Tf")) {
                    sscanf(linebuf, "%s %f", prefixbuf, &current_material->diffuse.w);
                }
                
                // Parse diffuse texture map
                else if(!strcmp(prefixbuf, "map_Kd")) {
                    char bmp_filepath[256];
                    char bmp_filename[128];
                    
                    sscanf(linebuf, "%s %s", prefixbuf, bmp_filename);
                    
                    strcpy(bmp_filepath, directory);
                    strcat(bmp_filepath, bmp_filename);
                    
                    Texture *new_tex = new Texture(bmp_filepath);
                    
                    glGenTextures(1, &current_material->gl_tex_id);
                    glBindTexture(GL_TEXTURE_2D, current_material->gl_tex_id);
                    
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                    
                    GLenum internalformat;
                    GLenum format;
                    
                    switch(new_tex->bytes_per_pixel) {
                    default:
                        internalformat = GL_RGB;
                        format = GL_BGR;
                        break;
                    case 4:
                        internalformat = GL_RGBA;
                        format = GL_BGRA;
                        break;
                    }
                    
                    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, new_tex->width, new_tex->height,
                        0, format, GL_UNSIGNED_BYTE, new_tex->data);
                    
                    glGenerateMipmap(GL_TEXTURE_2D);
                    
                    current_material->diffuse = vec4(1, 1, 1, 1);
                    current_material->texture = new_tex;
                    
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
                
                // Parse specular color
                else if(!strcmp(prefixbuf, "Ks")) {
                    sscanf(linebuf, "%s %f %f %f", prefixbuf,
                        &current_material->specular.x,
                        &current_material->specular.y,
                        &current_material->specular.z
                        );
                }
            }
            
            fclose(mtl_file);
        }
        
        // Parse mesh groups
        else if(!strcmp(prefixbuf, "g")) {
            static_mesh_group new_group;
            
            sscanf(linebuf, "%s %s", prefixbuf, new_group.name);
            
            bool is_existing = false;
            
            for(unsigned int i = 0; i < groups.size(); i++) {
                if(!strcmp(new_group.name, groups[i].name)) {
                    current_group = &groups[i];
                    is_existing = true;
                    break;
                }
            }
            
            if(!is_existing) {
                groups.push_back(new_group);
                current_group = &groups[groups.size() - 1];
            }
        }
        
        // Parse vertices
        else if(!strcmp(prefixbuf, "v")) {
            vec3 vertex;
            sscanf(linebuf, "%s %f %f %f", prefixbuf, &vertex.x, &vertex.y, &vertex.z);
            vertices.push_back(vertex);
        }
        
        // Parse texcoords
        else if(!strcmp(prefixbuf, "vt")) {
            vec2 uv;
            sscanf(linebuf, "%s %f %f", prefixbuf, &uv.x, &uv.y);
            uvs.push_back(uv);
        }
        
        // Parse normals
        else if(!strcmp(prefixbuf, "vn")) {
            vec3 normal;
            sscanf(linebuf, "%s %f %f %f", prefixbuf, &normal.x, &normal.y, &normal.z);
            normals.push_back(normal);
        }
        
        // Parse submesh
        else if(!strcmp(prefixbuf, "usemtl")) {
            char material_name[128];
            sscanf(linebuf, "%s %s", prefixbuf, material_name);
            
            unsigned int material_index = 0;
            
            for(unsigned int i = 0; i < materials.size(); i++) {
                if(!strcmp(materials[i].name, material_name)) {
                    material_index = i;
                    break;
                }
            }
            
            bool is_existing = false;
            
            for(unsigned int i = 0; i < current_group->submeshes.size(); i++) {
                if(current_group->submeshes[i].material_index == material_index) {
                    current_submesh = &current_group->submeshes[i];
                    is_existing = true;
                    break;
                }
            }
            
            if(!is_existing) {
                static_mesh_submesh new_submesh;
                new_submesh.material_index = material_index;
                new_submesh.num_faces = 0;
                
                current_group->submeshes.push_back(new_submesh);
                current_submesh = &current_group->submeshes[current_group->submeshes.size() - 1];
            }
        }
        
        // Parse faces
        else if(!strcmp(prefixbuf, "f")) {
            unsigned int vertex_index[3], uv_index[3], normal_index[3];
            
            sscanf(linebuf, "%s %d/%d/%d %d/%d/%d %d/%d/%d", prefixbuf,
                &vertex_index[0], &uv_index[0], &normal_index[0],
                &vertex_index[1], &uv_index[1], &normal_index[1],
                &vertex_index[2], &uv_index[2], &normal_index[2]
                );
            
            for(int i = 0; i < 3; i++) {
                // Face indices start at 1, so we adjust accordingly
                current_submesh->vertex_indices.push_back(vertex_index[i] - 1);
                current_submesh->uv_indices.push_back(uv_index[i] - 1);
                current_submesh->normal_indices.push_back(normal_index[i] - 1);
            }
            
            current_submesh->num_faces++;
        }
    }
    
    fclose(obj_file);
}

//----------------------------------------------------------------
// Name: Draw
// Desc: Sends the mesh to OpenGL to be drawn on-screen
//----------------------------------------------------------------
void StaticMesh::Draw() {
    for(unsigned int i = 0; i < groups.size(); i++) {
        for(unsigned int j = 0; j < groups[i].submeshes.size(); j++) {
            unsigned int cur_mat_index = groups[i].submeshes[j].material_index;
            
            glMaterialfv(GL_FRONT, GL_DIFFUSE, &materials[cur_mat_index].diffuse[0]);
            glMaterialfv(GL_FRONT, GL_AMBIENT, &materials[cur_mat_index].ambient[0]);
            glMaterialfv(GL_FRONT, GL_SPECULAR, &materials[cur_mat_index].specular[0]);
            
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, materials[cur_mat_index].gl_tex_id);
            
            for(unsigned int k = 0; k < groups[i].submeshes[j].num_faces; k++) {
                glBegin(GL_TRIANGLES);
                    glNormal3fv(&normals[groups[i].submeshes[j].normal_indices[k*3]][0]);
                    glTexCoord2fv(&uvs[groups[i].submeshes[j].uv_indices[k*3]][0]);
                    glVertex3fv(&vertices[groups[i].submeshes[j].vertex_indices[k*3]][0]);
                    
                    glNormal3fv(&normals[groups[i].submeshes[j].normal_indices[k*3+1]][0]);
                    glTexCoord2fv(&uvs[groups[i].submeshes[j].uv_indices[k*3+1]][0]);
                    glVertex3fv(&vertices[groups[i].submeshes[j].vertex_indices[k*3+1]][0]);
                    
                    glNormal3fv(&normals[groups[i].submeshes[j].normal_indices[k*3+2]][0]);
                    glTexCoord2fv(&uvs[groups[i].submeshes[j].uv_indices[k*3+2]][0]);
                    glVertex3fv(&vertices[groups[i].submeshes[j].vertex_indices[k*3+2]][0]);
                glEnd();
            }
            
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
        }
    }
}

StaticMesh::~StaticMesh() {
    for(unsigned int i = 0; i < materials.size(); i++) {
        if(materials[i].texture != nullptr) {
            glDeleteTextures(1, &materials[i].gl_tex_id);
            
            delete materials[i].texture;
        }
    }
}
