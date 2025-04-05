#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

#define VIDEO_ADDRESS (uint8_t*) 0xa0000
#define MAX_ROWS 200
#define MAX_COLS 320
#define PIXEL_LENGTH 1

/* Screen i/o ports */
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

/* Public kernel API */
void clear_screen();
void set_pixel(int col, int row, uint8_t colour);
void print_image(int col, int row, int height, int length, unsigned const int* bitmap);
void get_bitmap(int x, int y, unsigned int* out_tile);
void set_cursor_offset(int offset);
#endif
