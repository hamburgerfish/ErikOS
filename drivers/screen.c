#include "screen.h"
#include "../cpu/port.h"
#include "../libc/mem.h"
#include <stdint.h>


void set_cursor_offset(int offset);
void set_pixel(int col, int row, uint8_t colour);
int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);

//
// innermost print function for kernel, directly accesing the video memory
void set_pixel(int col, int row, uint8_t colour) {
	uint8_t *vidmem = (uint8_t*) VIDEO_ADDRESS;
	// error control: print a red pixel
	if (col >= MAX_COLS || row >= MAX_ROWS) {
		vidmem[(2*MAX_COLS)*(2*MAX_ROWS)] = 4;
	}

	int offset;
	offset = get_offset(col, row);
	int i;
	int j;
	for (i=0;i<PIXEL_LENGTH;i++) {
		for (j=0;j<PIXEL_LENGTH;j++) {
			offset = get_offset(col+i, row+j);
			vidmem[offset] = colour;
		}
	}
}

void print_image(int col, int row, int height, int length, unsigned const int* bitmap) {
	int y;
	int x;
	int colour;
	for (y=0;y<height;y++) {
		for (x=0;x<length;x++) {
			colour = bitmap[y*length + x];
			if (colour != 16){
				set_pixel(col+x,row+y, colour);
			}
		}
	}
}

void get_bitmap(int x, int y, unsigned int* out_tile) {
	int c = 0;
	int tile_y;
	int tile_x;
	for (tile_y=y; tile_y<y+16; tile_y++) {
		for (tile_x=x; tile_x<x+16; tile_x++) {
			out_tile[c++] = *(VIDEO_ADDRESS+ tile_y*MAX_COLS + tile_x);
		}
	}
}

void clear_screen() {
	int screen_size = MAX_COLS * MAX_ROWS;
	int i;
	uint8_t *screen = (uint8_t*) VIDEO_ADDRESS;

	for (i = 0; i < screen_size*4; i++) {
		screen[i] = 7;
	}
}



int get_offset(int col, int row) { return  (row * MAX_COLS + col); }
int get_offset_row(int offset) { return offset / MAX_COLS; }
int get_offset_col(int offset) { return (offset - (get_offset_row(offset)*MAX_COLS)); }
