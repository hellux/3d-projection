#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <png.h>

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
struct Pixel* get_pixel(struct Bitmap* bitmap, size_t x, size_t y);
void set_pixel(struct Bitmap* bitmap, size_t x, size_t y, uint8_t col[]);
void save_bitmap_to_png(struct Bitmap *bitmap, char* png_file_path);
