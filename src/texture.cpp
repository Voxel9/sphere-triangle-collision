#include "texture.h"

struct bitmap_file_header {
    char ident[2];
    char file_size[4];
    char reserve_1[2];
    char reserve_2[2];
    char pix_array_offset[4];
};

struct bitmap_info_header {
    char size[4];
    char width[4];
    char height[4];
    char color_plain[2];
    char bpp[2];
    char compression_method[4];
    char img_size[4];
    char horizontal_res[4];
    char vertical_res[4];
    char num_colors[4];
    char important_colors[4];
};

Texture::Texture(const char *filepath) {
    FILE *bmp_file = fopen(filepath, "rb");
    
    if(!bmp_file) {
        printf("Could not open cubemap BMP file:\n%s\n", filepath);
        return;
    }
    
    bitmap_file_header bmp_header;
    fread(&bmp_header, sizeof(bmp_header), 1, bmp_file);
    
    bitmap_info_header bmp_info_header;
    fread(&bmp_info_header, sizeof(bmp_info_header), 1, bmp_file);
    
    width = *((unsigned int *)&bmp_info_header.width);
    height = *((unsigned int *)&bmp_info_header.height);
    bytes_per_pixel = *((unsigned short *)&bmp_info_header.bpp) / 8;
    
    unsigned int pitch = width * bytes_per_pixel;
    
    data = new unsigned char[height * pitch];
    
    // Read line-by-line, from bottom to top, as BMPs are stored vertically flipped
    for(unsigned int yline = 0; yline < height; yline++) {
        fseek(bmp_file, -(yline * pitch) - pitch, SEEK_END);
        fread(data + (yline * pitch), pitch, 1, bmp_file);
    }
    
    fclose(bmp_file);
}

Texture::~Texture() {
    delete data;
}
