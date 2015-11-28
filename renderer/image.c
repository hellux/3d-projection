#include "image.h"

struct Bitmap* bitmap_create(int res[]) {
     struct Bitmap* B = malloc(sizeof(struct Bitmap));

     B->width = res[0];
     B->height = res[1];
     B->pixels = malloc(sizeof(struct Pixel)*res[0]*res[1]);

     return B;
}

struct Pixel* get_pixel(struct Bitmap* bitmap, size_t x, size_t y) {
    return bitmap->pixels + bitmap->width * (bitmap->height-y) + x;
}

void set_pixel(struct Bitmap* bitmap, size_t x, size_t y, uint8_t col[]) {
    get_pixel(bitmap, x, bitmap->height-y)->red = col[0];
    get_pixel(bitmap, x, bitmap->height-y)->green = col[1];
    get_pixel(bitmap, x, bitmap->height-y)->blue = col[2];
}

bool save_bitmap_to_png(struct Bitmap *bitmap, char* png_file_path) {
    FILE* f_png;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte** row_pointers = NULL;
    f_png = fopen(png_file_path, "wb");
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info_ptr = png_create_info_struct(png_ptr);
    
    if (f_png == NULL) {
        printf("Failed to open file '%s' for output.\n", png_file_path);
        return false;
    }
    
    /* Set image attributes. */
    png_set_IHDR(png_ptr, info_ptr,
                 bitmap->width, bitmap->height,
                 PNG_BIT_DEPTH,
                 PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    
    /* Initialize rows of PNG. */
    row_pointers = png_malloc(png_ptr, bitmap->height * sizeof(png_byte *));
    for (y = 0; y < bitmap->height; y++) {
        png_byte *row = png_malloc(png_ptr, 
                                   sizeof(uint8_t)*bitmap->width*PIXEL_SIZE);
        row_pointers[y] = row;
        for (x = 0; x < bitmap->width; x++) {
            struct Pixel* pixel = get_pixel(bitmap, x, y);
            *row++ = pixel->red;
            *row++ = pixel->green;
            *row++ = pixel->blue;
        }
    }

    /* Write the image data to "f_png". */
    png_init_io(png_ptr, f_png);
    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    for (y = 0; y < bitmap->height; y++) {
        png_free(png_ptr, row_pointers[y]);
    }
    png_free(png_ptr, row_pointers);
    fclose(f_png);
    return true;
}
