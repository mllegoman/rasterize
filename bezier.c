#include <stdint.h>
#define u_char unsigned char

int adjpow(uint32_t n) {
char i = 31;
	while ((n&((~0)<<i))==0) {
	i--;
	}
i--;
return ((n>>i)+((n>>i)&1))<<i;
}

void drawcircle(uint32_t x, 
		uint32_t y, 
		uint32_t radius, 
		uint16_t max, 
		uint8_t r, 
		uint8_t g, 
		uint8_t b, 
		uint8_t *rmap, 
		uint8_t *gmap, 
		uint8_t *bmap) {
int32_t rt = 0;
uint32_t ry;
int32_t i = 0;
	while (rt < radius) {
	ry = 0;
		while (ry*ry < radius*radius-rt*rt) {
		i = 0 - rt;
			while (i < rt) {
			rmap[(y-ry)*max+x+i] = r;
			gmap[(y-ry)*max+x+i] = g;
			bmap[(y-ry)*max+x+i] = b;
			rmap[(y+ry)*max+x+i] = r;
			gmap[(y+ry)*max+x+i] = g;
			bmap[(y+ry)*max+x+i] = b;
			i++;
			}
		ry++;
		}
	rt++;
	}
}

void bezier (   uint16_t *x,
		uint16_t *y, 
		u_char n,
		uint32_t width,  
		uint32_t tif, 
		uint16_t max, 
		u_char r,
		u_char g,
		u_char b,
		u_char *rmap, 
		u_char *gmap, 
		u_char *bmap) {
u_char i, j;
uint32_t slice[20];
uint32_t xi, yi;
double t, tc, tr, tg, tb, snbl, totx, toty;
tc = (double)1.0/adjpow(tif);
snbl = 1;

slice[0] = 1;
	for (i = 1; i < (n + 1)/2; i++) {
	slice[i] = slice[i - 1] * (n - i)/i;
	}

	for (i = n - 1; i > (n - 1)/2; i--) {
	slice[i] = slice[n - i - 1];
	}

	for (t = 0; (uint32_t)t==0; t+=tc+tc*(t>0.25||t>0.75)) {
		totx=0;
		toty=0;
		i  = 0;
		while (i < n) { // n-1 times (for each term)
		snbl = 1;
			for (j = 0; j < n - i - 1; j++) {
			snbl*=(1 - t);
			}
			for (j = 0; j < i; j++) {
			snbl*=t;
			}

		snbl*=slice[i];

		totx+=x[i]*snbl;
		toty+=y[i]*snbl;
		i++;
		}
	xi = (uint32_t)totx;
	yi = (uint32_t)toty;
		if (width > 1) {
		tr = 255 - r + r * t;
		tg = 255 - g + g * t;
		tb = 255 - b + b * t;
		//drawcircle(xi, yi, max, width, (uint8_t)tr, (uint8_t)tg, (uint8_t)tb, rmap, gmap, bmap);
		}
	rmap[yi*max+xi] = r;
	gmap[yi*max+xi] = g;
	bmap[yi*max+xi] = b;
	}
}

