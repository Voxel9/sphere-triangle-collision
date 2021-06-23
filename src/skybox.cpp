#include "skybox.h"

static const vec3 skybox_vertices[] = {
    { -0.5f, -0.5f,  0.5f },
    { -0.5f, -0.5f, -0.5f },
    { -0.5f,  0.5f, -0.5f },
    { -0.5f,  0.5f,  0.5f },
    {  0.5f, -0.5f, -0.5f },
    {  0.5f, -0.5f,  0.5f },
    {  0.5f,  0.5f,  0.5f },
    {  0.5f,  0.5f, -0.5f },
};

static const unsigned int skybox_indices[] = {
    4, 5, 6, 7,
    0, 1, 2, 3,
    2, 7, 6, 3,
    0, 5, 4, 1,
    1, 4, 7, 2,
    5, 0, 3, 6,
};

static const char *cubemap_filenames[] = {
    "data/Skybox/px.bmp",
    "data/Skybox/nx.bmp",
    "data/Skybox/py.bmp",
    "data/Skybox/ny.bmp",
    "data/Skybox/pz.bmp",
    "data/Skybox/nz.bmp",
};

//------------------------------------------------------------------------------------
// Name: Skybox
// Desc: Constructor for the Skybox class.
//       Reads a list of hardcoded bitmap image files as one whole cubemap texture
//------------------------------------------------------------------------------------
Skybox::Skybox() {
    glGenTextures(1, &cubetex_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubetex_id);
    
    // Load the cube bitmaps from files
    for(int i = 0; i < 6; i++) {
        FILE *cubetex_file = fopen(cubemap_filenames[i], "rb");
        
        if(!cubetex_file) {
            printf("Could not open cubemap BMP file:\n%s\n", cubemap_filenames[i]);
            continue; // Just skip this face for now
        }
        
        // Read line-by-line, from bottom to top, as BMPs are stored vertically flipped
        for(int yline = 0; yline < 512; yline++) {
            fseek(cubetex_file, -(yline * 512 * 3) - sizeof(cubetex_data[0][0]), SEEK_END);
            fread(cubetex_data[i][yline], sizeof(cubetex_data[0][0]), 1, cubetex_file);
        }
        
        fclose(cubetex_file);
        
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 512, 512, 0, GL_BGR, GL_UNSIGNED_BYTE, cubetex_data[i]);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

//----------------------------------------------------------------
// Name: Draw
// Desc: Sends the skybox mesh to OpenGL to be drawn on-screen
//----------------------------------------------------------------
void Skybox::Draw() {
    // Don't write to the depth buffer! (Keeps skybox behind everything if drawn first)
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    
    glEnable(GL_TEXTURE_CUBE_MAP);
    glDisable(GL_LIGHTING);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubetex_id);
    
    for(int i = 0; i < 6; i++) {
        const vec3 *p1 = &skybox_vertices[skybox_indices[i*4]];
        const vec3 *p2 = &skybox_vertices[skybox_indices[i*4+1]];
        const vec3 *p3 = &skybox_vertices[skybox_indices[i*4+2]];
        const vec3 *p4 = &skybox_vertices[skybox_indices[i*4+3]];
        
        glBegin(GL_QUADS);
            glTexCoord3f(p1->x, p1->y, p1->z);
            glVertex3fv(&p1->x);
            
            glTexCoord3f(p2->x, p2->y, p2->z);
            glVertex3fv(&p2->x);
            
            glTexCoord3f(p3->x, p3->y, p3->z);
            glVertex3fv(&p3->x);
            
            glTexCoord3f(p4->x, p4->y, p4->z);
            glVertex3fv(&p4->x);
        glEnd();
    }
    
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_CUBE_MAP);
    
    glDepthMask(true);
    glEnable(GL_DEPTH_TEST);
}

//----------------------------------------------------------------
// Name: ~Skybox
// Desc: Deconstructor for the Skybox class
//----------------------------------------------------------------
Skybox::~Skybox() {
    glDeleteTextures(1, &cubetex_id);
}
