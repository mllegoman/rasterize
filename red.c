#include "main.h"
#include "bezier.h"

uint32_t needn = 5;
uint32_t needed[] = {0b001101101011000010111100001110000,    //maxp
			0b01100011011011010110000101110000,  //cmap
			0b01100111011011000111100101100110,  //glyf
			0b01101100011011110110001101100001,  //loca
			0b01101000011001010110000101100100}; //head
char where (char s[4]) {
char i = 0;
	while (i < needn) {
		if ((s[0]<<24)+(s[1]<<16)+(s[2]<<8)+s[3]==needed[i]) {
		return i;
		}
	i++;
	}
return needn;
}

int16_t ***getpts(uint16_t *ctr, int16_t ctr_num, FILE *F) {
int16_t ***pts = malloc(3*s_size);
uint32_t i, j, k, n, lin;
n = ctr[ctr_num - 1] + 1;
uint8_t repeat;
uint8_t flags;
uint8_t treat[n*2], r[n];
i = 0;
	while (i < n) { // read flags for each point
	flags = fgetc(F);
	r[i] = flags&1;
	treat[i*2  ] = (flags&2) + ((flags>>4)&1);
	treat[i*2+1] = ((flags>>1)&2) + ((flags>>5)&1);
		if ((flags>>3)&1) {
		repeat = fgetc(F);
			while (repeat) {
			repeat--;
			i++;
			r[i] = flags&1;
			treat[i*2  ] = (flags&2) + ((flags>>4)&1); 
			treat[i*2+1] = ((flags>>1)&2) + ((flags>>5)&1);
			}
		}
	i++;
	}
i = ctr_num;
	while (i - 1) { // convert ctr from indices to lengths
	i--;
	ctr[i] = ctr[i] - ctr[i - 1];
	}
ctr[0]++;
i = 0;
lin = 0;
	while (i < 3) {
	pts[i]      = malloc(ctr_num*s_size);
	j = 0;
	k = 0;
	lin = 0;
		while (j < ctr_num) {
		pts[i][j] = malloc(ctr[j]*2);
		pts[i][j][0] = pts[i][j-!!j][k-!!k];
		k = 0;
			while (k < ctr[j]) {
				if (i < 2) {
					if (treat[lin*2+i]==3) {
					pts[i][j][k] = pts[i][j][k-!!k] + fgetc(F);
					}
					if (treat[lin*2+i]==2) {
					pts[i][j][k] = pts[i][j][k-!!k] - fgetc(F);
					}
					if (treat[lin*2+i]==1) {
					pts[i][j][k] = pts[i][j][k-!!k];
					}
					if (treat[lin*2+i]==0) {
					pts[i][j][k] = pts[i][j][k-!!k] + (fgetc(F)<<8) + fgetc(F);
					}
				} else {
				pts[2][j][k] = r[lin];
				}
			k++;
			lin++; // keep track of flags linearly
			}
		j++;
		}
	i++;
	}
return pts;
}

void dumpinfo (int16_t ***pts, uint16_t *ctr, int16_t ctr_num, FILE *OUT) {
uint16_t i, j, k;
i = 0;
	while (i < 3) {
	j = 0;
		while (j < ctr_num) {
		k = 0;
			while (k < ctr[j]) {
			fprintf(OUT, "%d\n", pts[i][j][k]);
			k++;
			}
		j++;
		}
	i++;
	}
}

// directly modifies points (do not use before dumpinfo):
void rasterize (uint16_t width, 
		uint16_t height, 
		uint8_t r, 
		uint8_t g, 
		uint8_t b, 
		int16_t ***pts, 
		uint16_t *ctr, 
		int16_t *glyfdata, 
		FILE *OUT) {
fprintf(OUT, "P3 %d %d 255\n", width, height);
uint8_t *rmap, *gmap, *bmap;
rmap = malloc(height*width);
gmap = malloc(height*width);
bmap = malloc(height*width);
uint32_t i, j, k, l;
int16_t *x, *y;
i = 0;
	while (i < height*width) {
	rmap[i] = 0;
	gmap[i] = 0;
	bmap[i] = 0;
	i++;
	}

i = 0;
	while (i < glyfdata[0]) {
	j = 0;
	k = 0;
		while (j < ctr[i]) {
		pts[0][i][j] = (pts[0][i][j]-glyfdata[1])*width/glyfdata[3];  
		pts[1][i][j] = (pts[1][i][j]-glyfdata[2])*height/glyfdata[4];
			if (pts[2][i][j]&&k!=0) { // adjust pts by k + 1
			l = 0;
			x = &pts[0][i][j-k];
                        y = &pts[1][i][j-k];
			bezier(x, y, k + 1, 2, 50, width, r, g, b, rmap, gmap, bmap);
			k = 0;
			}
		j++;
		k++;
		}
	i++;
	}
i = 0;
	while (i < height*width) {
	fprintf(OUT, "%d %d %d ", rmap[i], gmap[i], bmap[i]);
	i++;
	}
free(rmap);
free(gmap);
free(bmap);
					//printf("%d %d; %d %d %d %d; %d %d\n", pts[0][i][j], pts[1][i][j], glyfdata[1], glyfdata[2], glyfdata[3], glyfdata[4], (pts[0][i][j]-glyfdata[1])*width/glyfdata[3], (pts[1][i][j]-glyfdata[2])*height/glyfdata[4]);
		//printf("raster: %d %d %d\n", pts[0][i][j], pts[1][i][j], pts[2][i][j]);
		///printf("raster: %d %d %d\n", pts[0][i][j], pts[1][i][j], pts[2][i][j]);
				//while (l < k) {
				//printf("x/y: %d %d %d\n", x[l], y[l], p[l]);
				//l++;
				//}
			//printf("%d\n", k + 1);
			//p = &pts[2][i][j-k];

}

int main (s_uint argc, char **argv) {
// most of the proceedings are just table lookups and whatnot
if (argc < 2) {
return 1;
}
FILE *F = fopen(argv[1], "r");
if (!F) { 
return 1;
}

uint32_t ntable, i, j;
fseek(F, 4, 0);
ntable = (fgetc(F)<<8) + fgetc(F);
fseek(F, 12, 0);
uint32_t associate[ntable];
uint32_t toffset[ntable];
uint32_t tlength[ntable];
uint32_t offset[needn];
uint32_t length[needn];
char tmp;

i = 0;
while (i < ntable) {
associate[i] = (fgetc(F)<<24) + (fgetc(F)<<16) + (fgetc(F)<<8) + fgetc(F);
fgetc(F);fgetc(F);fgetc(F);fgetc(F);
toffset[i] = (fgetc(F)<<24) + (fgetc(F)<<16) + (fgetc(F)<<8) + fgetc(F);
tlength[i] = (fgetc(F)<<24) + (fgetc(F)<<16) + (fgetc(F)<<8) + fgetc(F);
i++;
}

j = 0;
while (j < needn) {
i = 0;
	while (associate[i]!=needed[j]) {
	i++;
	}
	offset[j] = toffset[i];
	length[j] = tlength[i];
j++;
}

fseek(F, offset[where("head")] + 336, 0);
uint16_t flavor = fgetc(F)+fgetc(F);
fseek(F, offset[where("maxp")] + 4, 0);
uint16_t nglyph = (fgetc(F)<<8) + fgetc(F);
fseek(F, ftell(F) + 24, 0);
fseek(F, offset[where("loca")], 0);
uint32_t loca[nglyph+1];

i = 0;
if (flavor) {
	while (i < nglyph + 1) {
	loca[i] = (fgetc(F)<<24) + (fgetc(F)<<16) + (fgetc(F)<<8) + fgetc(F); // 32 bit
	i++;
	}
} else {
	while (i < nglyph + 1) {
	loca[i] = (fgetc(F)<<10) + (fgetc(F)<<2); // 16 bit in - 32 out
	i++;
	}
}

int16_t glyphs[nglyph + 1][5];
uint16_t t_len, pts; 
uint16_t *ctr[nglyph + 1];
int16_t ***points[nglyph + 1]; 

i = 0;
while (i < nglyph) {
fseek(F, offset[where("glyf")] + loca[i], 0);
j = 0;
	while (j < 5) {
	glyphs[i][j] = ((fgetc(F)<<8) | fgetc(F));
	j++;
	}
j = 0;
ctr[i] = malloc(2*glyphs[i][0]);
	while (j < glyphs[i][0]) {
	ctr[i][j] = (fgetc(F)<<8) + fgetc(F);
	j++;
	}
t_len = (fgetc(F)<<8) + fgetc(F);
fseek(F, ftell(F) + t_len, 0); // skip instructions
points[i] = getpts(ctr[i], glyphs[i][0], F); // this function worked, but I was fooling around with it unnecessarily 
					     // something weird is happening with the number of points per contour
					     // which is stored in ctr[] and modified in getpts()
i++;
}

fseek(F, offset[where("cmap")], 0); // "cmap" contains character encoding table(s)
				    // there are 3 in name.ttf and one is a basic ascii table
				    // so once rasterize() works, getting bitmaps for each character
				    // should be easy
fgetc(F);fgetc(F);
uint16_t cmaptn = (fgetc(F)<<8) + fgetc(F);
uint16_t PID[cmaptn];
uint16_t EID[cmaptn];
uint32_t s_table[cmaptn];
uint16_t format;
FILE *O;
char fname[8];

i = 0;
while (i < cmaptn) {
PID[i] = (fgetc(F)<<8) + fgetc(F);
EID[i] = (fgetc(F)<<8) + fgetc(F);
s_table[i] = offset[where("cmap")] + (fgetc(F)<<24) + 
                                     (fgetc(F)<<16) + 
                                     (fgetc(F)<<8) + fgetc(F);
i++;
}

i = 0;
while (i < cmaptn) { // spit out .red file without storing glyph indexing values
fseek(F, s_table[i], 0);
format = (fgetc(F)<<8) + fgetc(F);
//printf("%d: %d\n", i, format);
	if (format==0) { // just straight up ascii chart
	fgetc(F);fgetc(F); // length of table (so far useless)
	fgetc(F);fgetc(F); // language of table (so far useless)
	j = 0;
		while (j < 256) { // needs check for unused ascii or redifined characters
		t_len = fgetc(F);
		fname[0] = t_len/100%10 + 48;
		fname[1] = t_len/10%10 + 48;
		fname[2] = t_len%10 + 48;
		fname[3] = '.';
		fname[4] = 'p';
		fname[5] = 'b';
		fname[6] = 'm';
		fname[7] = 0;
		O = fopen(fname, "w");
			if (O) {
			printf("%d %s\n", O, fname);
			rasterize(100, 100, 255, 255, 255, points[j], ctr[j], glyphs[j], O);
			fclose(O);
			}
		j++;
		}
	}
	//fprintf(O, "%c%c", j, ':'); // arbitrary ':' for second character
	//dumpinfo(points[t_len], ctr[t_len], glyphs[t_len][0], O);
	if (format==2) {
	fgetc(F);fgetc(F); // length of table (so far useless)
        fgetc(F);fgetc(F); // language of table (so far useless)
	j = 0;
	// subheaders n' shit (this is a multilingual precursor to Unicode)
	}
	if (format==4) { // Unicode!
	
	}
	if (format > 6&&format < 14) { // get rid of "reserved" value
	fgetc(F);fgetc(F);
	}
//t_len = (fgetc(F)<<8) + fgetc(F);
i++;
}


fclose(F);
}

// ignore this; I got a little too frisky with my code one night and thought getpts() needed reworking
// if only I could go back in time and punch my younger self

/*
uint16_t ctrc[glyfdata[0]];

i = 0;
	while (i < glyfdata[0]) { // copy for later use
	ctrc[i] = ctr[i];
	i++;
	}

i = 0;
	while (i < glyfdata[0]) {
	j = 0;
	k = 0;
		while (j < ctrc[i]) { // for each set flag add another point in ctr[i] langth
		ctr[i]+=pts[2][i][j];
		j++;
		}
	ctr[i]-=2;
	parsepts[0][i] = malloc(ctr[i]*2);
	parsepts[1][i] = malloc(ctr[i]*2);
	parsepts[0][i][0] = pts[0][i][0];   // two start points equal
	parsepts[1][i][0] = pts[1][i][0];
	parsepts[0][i][ctr[i] - 1] = pts[0][i][ctrc[i] - 1]; // two end points equal
	parsepts[1][i][ctr[i] - 1] = pts[1][i][ctrc[i] - 1]; // mind array sizes
	j = 1;
	k = 1;
		while (k < ctr[i] - 1) {	// copy approriate points and duplicate others
			if (pts[2][i][k]) {
			parsepts[0][i][k] = pts[0][i][j];
			parsepts[1][i][k] = pts[1][i][j];
			k++;
			}
		parsepts[0][i][k] = pts[0][i][j];
		parsepts[1][i][k] = pts[1][i][j];
		j++;
		k++;
		}
	j = 0;
	k = 0;
		while (j < ctr[i] - 1) {
		k+=actpts[2][i][j];
		actpts[2][i][j] = actpts[2][i][j]&(k); // cancel out uneccessarily flagged points
							// i.e. 0 0 1 1 1 1 0 1 1 1 0 0 0 1 0 0 1 becomes
							//      0 0 0 1 0 1 0 0 1 0 0 0 0 1 0 0 1 to denote the importance of curve endpoints
		j++;
		}
	i++;
	}
*/
