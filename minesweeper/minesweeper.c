#include "minesweeper.h"
#include "../drivers/screen.h"
#include "../cpu/timer.h"
#include <stdint.h>

#include "../bitmaps/1.h"
#include "../bitmaps/2.h"
#include "../bitmaps/3.h"
#include "../bitmaps/4.h"
#include "../bitmaps/5.h"
#include "../bitmaps/6.h"
#include "../bitmaps/7.h"
#include "../bitmaps/cursor_flag.h"
#include "../bitmaps/cursor_norm.h"
#include "../bitmaps/dead.h"
#include "../bitmaps/empty.h"
#include "../bitmaps/flag.h"
#include "../bitmaps/mine_explode.h"
#include "../bitmaps/mine.h"
#include "../bitmaps/gap1.h"
#include "../bitmaps/gap2.h"
#include "../bitmaps/num_0.h"
#include "../bitmaps/num_1.h"
#include "../bitmaps/num_2.h"
#include "../bitmaps/num_3.h"
#include "../bitmaps/num_4.h"
#include "../bitmaps/num_5.h"
#include "../bitmaps/num_6.h"
#include "../bitmaps/num_7.h"
#include "../bitmaps/num_8.h"
#include "../bitmaps/num_9.h"
#include "../bitmaps/num_null.h"
#include "../bitmaps/shades.h"
#include "../bitmaps/smile.h"
#include "../bitmaps/tile.h"

//#include "../bitmaps/gap1.h"
#include "../bitmaps/logo.h"

static const unsigned int* NUMS[] = {num_0, num_1, num_2, num_3, num_4, num_5, num_6, num_7, num_8, num_9};
counter inc_counter(counter c);
static const unsigned int* CURSORS[] = {cursor_norm, cursor_flag};
static uint8_t isflag = 0;
static unsigned int selected_tile[768];
static uint8_t cursor_x;
static uint8_t cursor_y;


void get_bitmap(int x, int y);

	
uint32_t rng(uint32_t seed) {
	seed = seed * 1103515245 + 12345;
	return (uint32_t)(seed/65536) % 32768;
}

void init_board() {
	print_image(152 , HEADER_Y_OFFSET, SMILE_HEIGHT, SMILE_WIDTH, smile);
	for (int i=0; i<3; i++) {
		print_image(UNIT_SIZE*(1+i), HEADER_Y_OFFSET, NUM_NULL_HEIGHT, NUM_NULL_WIDTH, num_null);
		print_image(UNIT_SIZE*(BOARD_X-4+i), HEADER_Y_OFFSET, NUM_NULL_HEIGHT, NUM_NULL_WIDTH, num_null);
	}
	for (int grid_y=0; grid_y < BOARD_Y; grid_y++) {
		for (int grid_x=0; grid_x < BOARD_X; grid_x++) {
			print_image(grid_x*UNIT_SIZE, BOARD_Y_OFFSET + grid_y*UNIT_SIZE, TILE_HEIGHT, TILE_WIDTH, tile);

		}
	}
	cursor_x = 9;
	cursor_y = 5;
	get_bitmap(UNIT_SIZE*9, BOARD_Y_OFFSET + UNIT_SIZE*5);
	print_image(UNIT_SIZE*9, BOARD_Y_OFFSET + UNIT_SIZE*5, UNIT_SIZE, UNIT_SIZE, CURSORS[isflag]);
}

void start() {
	uint8_t alive = 1;
	counter stopwatch;
	uint32_t prev = tick; 
	stopwatch.hunds = 0;
	stopwatch.tens = 0;
	stopwatch.ones = 0;
	print_image(UNIT_SIZE*3, HEADER_Y_OFFSET, NUM_0_HEIGHT, NUM_0_WIDTH, num_0);
	print_image(UNIT_SIZE*2, HEADER_Y_OFFSET, NUM_0_HEIGHT, NUM_0_WIDTH, num_0);
	print_image(UNIT_SIZE*1, HEADER_Y_OFFSET, NUM_0_HEIGHT, NUM_0_WIDTH, num_0);
	uint8_t mine_ones = TOTAL_MINE_COUNT % 10;
	uint8_t mine_tens = (TOTAL_MINE_COUNT - mine_ones)/10;
	print_image(UNIT_SIZE*(BOARD_X-4), HEADER_Y_OFFSET, NUM_0_HEIGHT, NUM_0_WIDTH, num_0);
	print_image(UNIT_SIZE*(BOARD_X-3), HEADER_Y_OFFSET, UNIT_SIZE, UNIT_SIZE, NUMS[mine_tens]);
	print_image(UNIT_SIZE*(BOARD_X-2), HEADER_Y_OFFSET, UNIT_SIZE, UNIT_SIZE, NUMS[mine_ones]);
	while (alive == 1) {
		if (tick > prev+49) {
			stopwatch = inc_counter(stopwatch);
			prev = tick;
		}
	}
}

counter inc_counter(counter c) {
	if (c.ones == 9) {
		if (c.tens == 9) {
			if (c.hunds == 9) return c;
			else {
				c.hunds++;
				print_image(UNIT_SIZE*1, HEADER_Y_OFFSET, UNIT_SIZE, UNIT_SIZE, NUMS[c.hunds]);
			}
		c.tens = 0;
		print_image(UNIT_SIZE*2, HEADER_Y_OFFSET, NUM_0_HEIGHT, NUM_0_WIDTH, num_0);
		}
		else{
			c.tens++;
			print_image(UNIT_SIZE*2, HEADER_Y_OFFSET, UNIT_SIZE, UNIT_SIZE, NUMS[c.tens]);
		}
	c.ones = 0;
	print_image(UNIT_SIZE*3, HEADER_Y_OFFSET, NUM_0_HEIGHT, NUM_0_WIDTH, num_0);		
	}
	else {
		c.ones++;
		print_image(UNIT_SIZE*3, HEADER_Y_OFFSET, NUM_0_HEIGHT, NUM_0_WIDTH, NUMS[c.ones]);
	}
	return c;
}

void move_cursor(uint8_t dir) {
	uint8_t orig_cursor_x = cursor_x;
	uint8_t orig_cursor_y = cursor_y;
	if (cursor_y == 10) {
	       	if (dir != 0x1F) return;
		else cursor_y = 0;
	}
	else if ( (cursor_y == 0) & (dir == 0x11) ) cursor_y = 10;
	else if ( (cursor_y == 9) & (dir == 0x1F) ) return;
	else if ( (cursor_x == 0) & (dir == 0x1E) ) return;
	else if ( (cursor_x == 19) & (dir == 0x20) ) return;

	else if (dir == 0x11) cursor_y--;
	else if (dir == 0x1E) cursor_x--;
	else if (dir == 0x1F) cursor_y++;
	else if (dir == 0x20) cursor_x++;

	if (orig_cursor_y == 10) {
		print_image(152, HEADER_Y_OFFSET, UNIT_SIZE, UNIT_SIZE, selected_tile);
	}
	else {
		print_image(orig_cursor_x*UNIT_SIZE, BOARD_Y_OFFSET + orig_cursor_y*UNIT_SIZE, UNIT_SIZE, UNIT_SIZE, selected_tile);
	}
	if (cursor_y == 10) {
		get_bitmap(152, HEADER_Y_OFFSET);
		print_image(152, HEADER_Y_OFFSET, UNIT_SIZE, UNIT_SIZE, CURSORS[isflag]);
	}
	else {
		get_bitmap(cursor_x*UNIT_SIZE, BOARD_Y_OFFSET + cursor_y*UNIT_SIZE);
		print_image(cursor_x*UNIT_SIZE, BOARD_Y_OFFSET + cursor_y*UNIT_SIZE, UNIT_SIZE, UNIT_SIZE, CURSORS[isflag]);
	}
}

void get_bitmap(int x, int y) {
	int c = 0;
	for (int tile_y=0; tile_y<16; tile_y++) {
		for (int tile_x=0; tile_x<16; tile_x++) {
			selected_tile[c] = *(VIDEO_ADDRESS + (y+tile_y)*MAX_COLS + (x+tile_x));
			c++;
		}
	}
}


void show_logo() {
	print_image(160-32, 100-16, LOGO_HEIGHT, LOGO_WIDTH, logo);
}
