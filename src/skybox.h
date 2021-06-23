#pragma once

#include "main.h"

class Skybox {
public:
    Skybox();
    ~Skybox();
    
    void Draw();
private:
    GLuint cubetex_id;
    unsigned char cubetex_data[6][512][512][3];
};
