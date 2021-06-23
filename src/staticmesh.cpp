#include "staticmesh.h"

// -----------------------------------------------------------------------------------
// Name: StaticMesh
// Desc: Constructor for the StaticMesh class.
//       Employs a very basic OBJ model file importer to load a mesh
// -----------------------------------------------------------------------------------
StaticMesh::StaticMesh(const char *filepath) {
    FILE *obj_file = fopen(filepath, "r");
    
    if(!obj_file) {
        printf("Could not open OBJ file:\n%s\n", filepath);
    }
    
    num_faces = 0;
    
    char linebuf[128];
    
    while(fgets(linebuf, sizeof(linebuf), obj_file) != nullptr) {
        char prefixbuf[32];
        sscanf(linebuf, "%s", prefixbuf);
        
        if(!strcmp(prefixbuf, "v")) {
            vec3 vertex;
            sscanf(linebuf, "%s %f %f %f", prefixbuf, &vertex.x, &vertex.y, &vertex.z);
            vertices.push_back(vertex);
        }
        else if(!strcmp(prefixbuf, "vt")) {
            vec2 uv;
            sscanf(linebuf, "%s %f %f", prefixbuf, &uv.x, &uv.y);
            uvs.push_back(uv);
        }
        else if(!strcmp(prefixbuf, "vn")) {
            vec3 normal;
            sscanf(linebuf, "%s %f %f %f", prefixbuf, &normal.x, &normal.y, &normal.z);
            normals.push_back(normal);
        }
        else if(!strcmp(prefixbuf, "f")) {
            unsigned int vertex_index[3], uv_index[3], normal_index[3];
            
            sscanf(linebuf, "%s %d/%d/%d %d/%d/%d %d/%d/%d", prefixbuf,
                &vertex_index[0], &uv_index[0], &normal_index[0],
                &vertex_index[1], &uv_index[1], &normal_index[1],
                &vertex_index[2], &uv_index[2], &normal_index[2]
                );
            
            for(int i = 0; i < 3; i++) {
                // Face indices start at 1, so we adjust accordingly
                vertex_indices.push_back(vertex_index[i] - 1);
                uv_indices.push_back(uv_index[i] - 1);
                normal_indices.push_back(normal_index[i] - 1);
            }
            
            num_faces++;
        }
    }
    
    fclose(obj_file);
}

//----------------------------------------------------------------
// Name: Draw
// Desc: Sends the mesh to OpenGL to be drawn on-screen
//----------------------------------------------------------------
void StaticMesh::Draw() {
    for(unsigned int i = 0; i < num_faces; i++) {
        glBegin(GL_TRIANGLES);
            glNormal3fv(&normals[normal_indices[i*3]][0]);
            glVertex3fv(&vertices[vertex_indices[i*3]][0]);
            
            glNormal3fv(&normals[normal_indices[i*3+1]][0]);
            glVertex3fv(&vertices[vertex_indices[i*3+1]][0]);
            
            glNormal3fv(&normals[normal_indices[i*3+2]][0]);
            glVertex3fv(&vertices[vertex_indices[i*3+2]][0]);
        glEnd();
    }
}
