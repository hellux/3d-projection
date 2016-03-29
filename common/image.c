#include "image.h"

struct Bitmap* bitmap_create(int res[]) {
     struct Bitmap* B = malloc(sizeof(struct Bitmap));
     B->width = res[0];
     B->height = res[1];
     B->pixels = malloc(sizeof(struct Pixel)*res[0]*res[1]);
     return B;
}

struct Pixel* bitmap_get_pixel(struct Bitmap* bitmap, size_t x, size_t y) {
    return bitmap->pixels + bitmap->width*y + x;
}

void bitmap_set_pixel(struct Bitmap* bitmap, size_t x, size_t y, uint8_t color[]) {
    struct Pixel* pixel = bitmap_get_pixel(bitmap, x, bitmap->height-y-1);
    pixel->red = color[0];
    pixel->green = color[1];
    pixel->blue = color[2];
}

bool bitmap_save_png(struct Bitmap *bitmap, char* png_file_path) {
    FILE* f_png;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte** row_pointers = NULL;
    f_png = fopen(png_file_path, "wb");
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info_ptr = png_create_info_struct(png_ptr);

    if (!png_ptr) {
        fprintf(stderr, "image: 'png_create_write_struct' failed!\n");
    }
    if (!info_ptr) {
        fprintf(stderr, "image: 'png_create_info_struct' failed!\n");
    }
    if (f_png == NULL) {
        fprintf(stderr, "image: failed to open file '%s' for output.\n", png_file_path);
        return false;
    }
    
    /* set image attributes */
    png_set_IHDR(png_ptr, info_ptr,
                 bitmap->width, bitmap->height,
                 PNG_BIT_DEPTH,
                 PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    
    /* initialize rows of PNG */
    row_pointers = png_malloc(png_ptr, bitmap->height * sizeof(png_byte *));
    for (y = 0; y < bitmap->height; y++) {
        png_byte *row = png_malloc(png_ptr, 
                                   sizeof(uint8_t)*bitmap->width*PIXEL_SIZE);
        row_pointers[y] = row;
        for (x = 0; x < bitmap->width; x++) {
            struct Pixel* pixel = bitmap_get_pixel(bitmap, x, y);
            *row++ = pixel->red;
            *row++ = pixel->green;
            *row++ = pixel->blue;
        }
    }

    /* write the image data to file */
    png_init_io(png_ptr, f_png);
    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    printf("image: bitmap written to '%s'.\n", png_file_path);

    for (y = 0; y < bitmap->height; y++) {
        png_free(png_ptr, row_pointers[y]);
    }
    png_free(png_ptr, row_pointers);
    fclose(f_png);

    return true;
}

void bitmap_free(struct Bitmap* bitmap) {
    free(bitmap->pixels);
    free(bitmap);
}
