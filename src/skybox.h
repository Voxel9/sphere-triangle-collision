#pragma once

#include "main.h"
#include "texture.h"

class Skybox {
public:
    Skybox();
    ~Skybox();
    
    void Draw();
private:
    GLuint cubetex_id;
    Texture *cubetex_data[6];
};
