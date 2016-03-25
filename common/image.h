#ifndef image_h
#define image_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <png.h>

#define PNG_BIT_DEPTH 8
#define PIXEL_SIZE 3 

struct Pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

struct Bitmap {
    struct Pixel* pixels;
    size_t width;
    size_t height;
};

struct Bitmap* bitmap_create(int res[]);
struct Pixel* bitmap_get_pixel(struct Bitmap* bitmap, size_t x, size_t y);
void bitmap_set_pixel(struct Bitmap* bitmap, size_t x, size_t y, uint8_t col[]);
bool bitmap_save_png(struct Bitmap* bitmap, char* png_file_path);
void bitmap_free(struct Bitmap* bitmap);

#endif
