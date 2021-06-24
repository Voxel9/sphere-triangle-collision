#pragma once

#include "main.h"

class Texture {
public:
    Texture(const char *filepath);
    ~Texture();
    
    unsigned int width;
    unsigned int height;
    unsigned int bytes_per_pixel;
    
    unsigned char *data;
};
