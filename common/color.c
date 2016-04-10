#include "color.h"

void rgb_to_hsl(uint8_t rgb[], double hsl[]) {
	double R = rgb[0] / 255.0;
	double G = rgb[1] / 255.0;
	double B = rgb[2] / 255.0;
	double H, S, L, r, g, b;
	
	double c_min = MIN(MIN(R, G), B);
	double c_max = MAX(MAX(R, G), B);
	double c_d = c_max - c_min;

	L = (c_max+c_min)/2;

	if (c_d == 0) {
	   H = 0;
	   S = 0;
	}
	else {
		S = (L < 0.5) ? c_d / (c_max+c_min) : c_d / (2-c_max-c_min);
		r = (((c_max-R)/6) + (c_d/2)) / c_d;
		g = (((c_max-G)/6) + (c_d/2)) / c_d;
		b = (((c_max-B)/6) + (c_d/2)) / c_d;
		if (R == c_max) H = b-g;
		else if (G == c_max) H = (1/3.0) + r-b;
		else H = (2/3.0) + g-r;
		if (H<0) H += 1;
		if (H>1) H -= 1;
	}

	hsl[0] = H;
	hsl[1] = S;
	hsl[2] = L;	
}

void hsl_to_rgb(double hsl[], uint8_t rgb[]) {
	double v;
	double r, g, b;
	double H = hsl[0];
	double S = hsl[1];
	double L = hsl[2];

	r = g = b = L;
	v = (L <= 0.5) ? L+L*S : L+S-L*S;
    if (v > 0) {
        double m, sv, frac, vsf, mid1, mid2;
        int sextant;
        m = 2*L-v;
        sv = (v-m)/v;
        H *= 6.0;
        sextant = H;
        frac = H-sextant;
        vsf = v*sv*frac;
        mid1 = m + vsf;
        mid2 = v - vsf;
        switch (sextant) {
        case 0:
            r = v;
            g = mid1;
            b = m;
            break;
        case 1:
            r = mid2;
            g = v;
            b = m;
            break;
        case 2:
            r = m;
            g = v;
            b = mid1;
            break;
        case 3:
            r = m;
            g = mid2;
            b = v;
            break;
        case 4:
            r = mid1;
            g = m;
            b = v;
            break;
        case 5:
            r = v;
            g = m;
            b = mid2;
            break;
        }
    }
    rgb[0] = r * 0xFF;
    rgb[1] = g * 0xFF;
    rgb[2] = b * 0xFF;
}
