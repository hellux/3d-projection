#include <stdint.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

void rgb_to_hsl(uint8_t rgb[], double hsl[]);
void hsl_to_rgb(double hsl[], uint8_t rgb[]);
double hue_to_rgb(double v1, double v2, double vH);
