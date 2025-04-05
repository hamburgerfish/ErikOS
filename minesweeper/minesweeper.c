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

#include "../bitmaps/gap1.h"
#include "../bitmaps/gap2.h"

#include "../bitmaps/empty.h"
#include "../bitmaps/flag.h"
#include "../bitmaps/mine_explode.h"
#include "../bitmaps/mine.h"
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

#include "../bitmaps/logo.h"

static const unsigned int* NUMS[] = {num_0, num_1, num_2, num_3, num_4, num_5, num_6, num_7, num_8, num_9};
static const unsigned int* HIDDEN[] = {empty, _1, _2, _3, _4, _5, _6, _7, mine};
static const unsigned int* CURSOR = cursor_norm;
static unsigned int selected_tile[768];
static counter stopwatch;
static counter mine_counter;
static uint8_t flag_count = 0;
static uint32_t prev_tick;
static uint8_t cursor_x;
static uint8_t cursor_y;
static uint8_t pause = 1;
static uint8_t alive = 0;
static uint8_t reveal_count = 0;
static uint8_t exploded = 0;
static uint8_t board[200] = {0};
static uint8_t reveal_state[200] = {0};
static uint8_t surround[8];

uint32_t rng(uint32_t seed);
counter inc_counter(counter c, uint8_t start_tile);
counter dec_counter(counter c, uint8_t start_tile);
void assign_values();
uint8_t get_board_x(uint8_t offset);
uint8_t get_board_y(uint8_t offset);
uint8_t get_board_offset(uint8_t x, uint8_t y);


uint32_t rng(uint32_t seed) {
	seed = seed * 1103515245 + 12345;
	return (uint32_t)(seed/65536) % 32768;
}

void init_board() {
	pause = 1;
	CURSOR = cursor_norm;
	flag_count = 0;
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
	get_bitmap(UNIT_SIZE*9, BOARD_Y_OFFSET + UNIT_SIZE*5, selected_tile);
	print_image(UNIT_SIZE*9, BOARD_Y_OFFSET + UNIT_SIZE*5, UNIT_SIZE, UNIT_SIZE, CURSOR);
	pause = 0;
}

void start() {
	pause = 1;
	reveal_count = 0;
	prev_tick = tick; 
	stopwatch.hunds = 0;
	stopwatch.tens = 0;
	stopwatch.ones = 0;
	uint8_t mine_counter_ones;
	uint8_t mine_counter_tens;
	uint8_t mine_counter_hunds;
	mine_counter_ones = TOTAL_MINE_COUNT % 10;
	mine_counter_tens = ((TOTAL_MINE_COUNT % 100) - mine_counter_ones)/10;
	mine_counter_hunds = ((TOTAL_MINE_COUNT % 1000) - mine_counter_tens - mine_counter_ones)/100;
	mine_counter.hunds = mine_counter_hunds;
	mine_counter.tens = mine_counter_tens;
	mine_counter.ones = mine_counter_ones;
	print_image(UNIT_SIZE*3, HEADER_Y_OFFSET, NUM_0_HEIGHT, NUM_0_WIDTH, num_0);
	print_image(UNIT_SIZE*2, HEADER_Y_OFFSET, NUM_0_HEIGHT, NUM_0_WIDTH, num_0);
	print_image(UNIT_SIZE*1, HEADER_Y_OFFSET, NUM_0_HEIGHT, NUM_0_WIDTH, num_0);
	print_image(UNIT_SIZE*(BOARD_X-4), HEADER_Y_OFFSET, UNIT_SIZE, UNIT_SIZE, NUMS[mine_counter_hunds]);
	print_image(UNIT_SIZE*(BOARD_X-3), HEADER_Y_OFFSET, UNIT_SIZE, UNIT_SIZE, NUMS[mine_counter_tens]);
	print_image(UNIT_SIZE*(BOARD_X-2), HEADER_Y_OFFSET, UNIT_SIZE, UNIT_SIZE, NUMS[mine_counter_ones]);
	assign_values();
	pause = 0;
	alive = 1;
}

void init_counter() {
	while(1) {
		if ((alive == 1) & (tick > prev_tick+49)) {
			stopwatch = inc_counter(stopwatch, 1);
			prev_tick = tick;
		}
	}
}

uint8_t get_starting_offset(uint8_t center_x, uint8_t center_y) {
	uint8_t start_x;
	uint8_t start_y;
	if (center_y == 0) start_y = 0;
	else start_y = center_y - 1;
	if (center_x == 0) start_x = 0;
	else start_x = center_x - 1;
	return get_board_offset(start_x, start_y);
}

uint8_t get_surround(uint8_t center_x, uint8_t center_y, uint8_t* surround_arr) {
	uint8_t start_offset = get_starting_offset(center_x, center_y);
	uint8_t curr_x = get_board_x(start_offset);
	uint8_t curr_y = get_board_y(start_offset);
	uint8_t start_x = curr_x;
	uint8_t array_ind = 0;
	for (uint8_t i=0; i<9; i++) {
		if ((curr_x != center_x) || (curr_y != center_y)) {
			surround_arr[array_ind++] = get_board_offset(curr_x, curr_y);
		}
		if ((curr_x == BOARD_X-1) || (curr_x == center_x+1)) {
			if ((curr_y == BOARD_Y-1) || (curr_y == center_y+1)) return array_ind;
			else {
				curr_x = start_x;
				curr_y++;
			}
		}
		else curr_x++;
	}
	return 0;
}

void assign_values() {
	uint8_t i = 0;
	uint32_t next_seed = tick;
	uint8_t curr_num;
	uint8_t cursor_offset = get_board_offset(cursor_x, cursor_y);
	for (uint8_t j=0; j<200; j++) {
		board[j] = 0;
		reveal_state[j] = 0;
	}
	while (i < TOTAL_MINE_COUNT) {
		next_seed = rng(next_seed);
	        curr_num = next_seed % 200;
		if ((board[curr_num] != 8) & (curr_num != cursor_offset)) {
			board[curr_num] = 8;
			i++;
		}
	}
	uint8_t x;
	uint8_t y;
	uint8_t c;
	uint8_t mine_count;
	for (y=0; y<BOARD_Y; y++) {
		for (x=0; x<BOARD_X; x++) {
			if (board[get_board_offset(x, y)] != 8) {
				c = get_surround(x, y, surround);
				mine_count = 0;
				for (i=0; i<c; i++) {
					if (board[surround[i]] == 8) mine_count++;
				}
				board[get_board_offset(x, y)] = mine_count;
			}
		}
	}
}

void reveal_surround(uint8_t x, uint8_t y) {
	uint8_t curr_val = board[get_board_offset(x, y)];
	print_image(x*UNIT_SIZE, BOARD_Y_OFFSET + y*UNIT_SIZE, UNIT_SIZE, UNIT_SIZE, HIDDEN[curr_val]);
	if ((reveal_state[get_board_offset(x, y)] != 1) & (curr_val != 8)) reveal_count++;
	reveal_state[get_board_offset(x, y)] = 1;
	if (reveal_count == BOARD_X*BOARD_Y - TOTAL_MINE_COUNT) {
		alive = 0;
		exploded = 1;
		print_image(152, HEADER_Y_OFFSET, SHADES_WIDTH, SHADES_HEIGHT, shades);
	}
	if (curr_val != 0) return;
	uint8_t c = get_surround(x, y, surround);
	uint8_t curr_surround[9];
	uint8_t i;
	for (i=0; i<c; i++) curr_surround[i] = surround[i];
	for (i=0; i<c; i++) {
		uint8_t curr_offset = curr_surround[i];
		if (reveal_state[curr_offset] == 0) {
			uint8_t curr_x = get_board_x(curr_offset);
			uint8_t curr_y = get_board_y(curr_offset);
			reveal_surround(curr_x, curr_y);
		}
	}
}

void reveal_mines() {
	uint8_t x;
	uint8_t y;
	for (uint8_t i=0; i<200; i++) {
		if (board[i] == 8) {
			x = get_board_x(i);
			y = get_board_y(i);
			reveal_surround(x, y);
		}
	}
}

void set_flag() {
	uint8_t curr_offset = get_board_offset(cursor_x, cursor_y);
	if (reveal_state[curr_offset] == 1) return;
	else if (reveal_state[curr_offset] == 0) {
		print_image(cursor_x*UNIT_SIZE, BOARD_Y_OFFSET + cursor_y*UNIT_SIZE, FLAG_WIDTH, FLAG_HEIGHT, flag);
		get_bitmap(cursor_x*UNIT_SIZE, BOARD_Y_OFFSET + cursor_y*UNIT_SIZE, selected_tile);
		reveal_state[curr_offset] = 2;
		mine_counter = dec_counter(mine_counter, BOARD_X-4);
		flag_count++;
	}
	else {
		print_image(cursor_x*UNIT_SIZE, BOARD_Y_OFFSET + cursor_y*UNIT_SIZE, TILE_WIDTH, TILE_HEIGHT, tile);
		get_bitmap(cursor_x*UNIT_SIZE, BOARD_Y_OFFSET + cursor_y*UNIT_SIZE, selected_tile);
		reveal_state[curr_offset] = 0;
		if (flag_count <= TOTAL_MINE_COUNT) mine_counter = inc_counter(mine_counter, BOARD_X-4);
		flag_count--;
	}
	print_image(cursor_x*UNIT_SIZE, BOARD_Y_OFFSET + cursor_y*UNIT_SIZE, UNIT_SIZE, UNIT_SIZE, CURSOR);
}

void reveal() {
	if (pause == 1) return;
	if (cursor_y == 10) {
		alive = 0;
		exploded = 0;
		init_board();
	}
	else {
		if (alive == 0) {
			if (exploded == 0) {
				start();
				if (CURSOR == cursor_flag) {
					set_flag();
					return;
				}
			}
			else {
				init_board();
				exploded = 0;
				return;
			}
		}
		if (CURSOR == cursor_flag) {
			set_flag();
			return;
		}
		uint8_t tile_val = board[get_board_offset(cursor_x, cursor_y)];
		if (tile_val == 8) {
			reveal_mines();
			print_image(cursor_x*UNIT_SIZE, BOARD_Y_OFFSET + cursor_y*UNIT_SIZE, MINE_EXPLODE_WIDTH, MINE_EXPLODE_HEIGHT, mine_explode);
			get_bitmap(cursor_x*UNIT_SIZE, BOARD_Y_OFFSET + cursor_y*UNIT_SIZE, selected_tile);
			print_image(152, HEADER_Y_OFFSET, DEAD_WIDTH, DEAD_HEIGHT, dead);
			alive = 0;
			exploded = 1;
			return;
		}
		reveal_surround(cursor_x, cursor_y);
		get_bitmap(cursor_x*UNIT_SIZE, BOARD_Y_OFFSET + cursor_y*UNIT_SIZE, selected_tile);
		print_image(cursor_x*UNIT_SIZE, BOARD_Y_OFFSET + cursor_y*UNIT_SIZE, UNIT_SIZE, UNIT_SIZE, CURSOR);
	}
}

counter inc_counter(counter c, uint8_t start_tile) {
	if (c.ones == 9) {
		if (c.tens == 9) {
			if (c.hunds == 9) return c;
			else {
				c.hunds++;
				print_image(UNIT_SIZE*start_tile, HEADER_Y_OFFSET, UNIT_SIZE, UNIT_SIZE, NUMS[c.hunds]);
			}
		c.tens = 0;
		print_image(UNIT_SIZE*(start_tile+1), HEADER_Y_OFFSET, NUM_0_HEIGHT, NUM_0_WIDTH, num_0);
		}
		else{
			c.tens++;
			print_image(UNIT_SIZE*(start_tile+1), HEADER_Y_OFFSET, UNIT_SIZE, UNIT_SIZE, NUMS[c.tens]);
		}
	c.ones = 0;
	print_image(UNIT_SIZE*(start_tile+2), HEADER_Y_OFFSET, NUM_0_HEIGHT, NUM_0_WIDTH, num_0);		
	}
	else {
		c.ones++;
		print_image(UNIT_SIZE*(start_tile+2), HEADER_Y_OFFSET, NUM_0_HEIGHT, NUM_0_WIDTH, NUMS[c.ones]);
	}
	return c;
}

counter dec_counter(counter c, uint8_t start_tile) {
	if (c.ones == 0) {
		if (c.tens == 0) {
			if (c.hunds == 0) return c;
			else {
				c.hunds--;
				print_image(start_tile*UNIT_SIZE, HEADER_Y_OFFSET, UNIT_SIZE, UNIT_SIZE, NUMS[c.hunds]);
			}
		c.tens = 9;
		print_image((start_tile+1)*UNIT_SIZE, HEADER_Y_OFFSET, NUM_9_WIDTH, NUM_9_HEIGHT, num_9);
		}
		else {
			c.tens--;
			print_image((start_tile+1)*UNIT_SIZE, HEADER_Y_OFFSET, UNIT_SIZE, UNIT_SIZE, NUMS[c.tens]);
		}
	c.ones = 9;
	print_image((start_tile+2)*UNIT_SIZE, HEADER_Y_OFFSET, NUM_9_WIDTH, NUM_9_HEIGHT, num_9);
	}
	else {
		c.ones--;
		print_image((start_tile+2)*UNIT_SIZE, HEADER_Y_OFFSET, UNIT_SIZE, UNIT_SIZE, NUMS[c.ones]);
	}
	return c;
}

void switch_mode() {
	if (pause == 1) return;
	if (CURSOR == cursor_norm) CURSOR = cursor_flag;
	else CURSOR = cursor_norm;
	int p_x;
	int p_y;
	if (cursor_y == 10) {
		p_x = 152;
		p_y = HEADER_Y_OFFSET;
	}
	else {
		p_x = cursor_x*UNIT_SIZE;
		p_y = BOARD_Y_OFFSET + cursor_y*UNIT_SIZE;
	}
	print_image(p_x, p_y, UNIT_SIZE, UNIT_SIZE, CURSOR);
}

void move_cursor(uint8_t dir) {
	if (pause == 1) return;
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
		get_bitmap(152, HEADER_Y_OFFSET, selected_tile);
		print_image(152, HEADER_Y_OFFSET, UNIT_SIZE, UNIT_SIZE, CURSOR);
	}
	else {
		get_bitmap(cursor_x*UNIT_SIZE, BOARD_Y_OFFSET + cursor_y*UNIT_SIZE, selected_tile);
		print_image(cursor_x*UNIT_SIZE, BOARD_Y_OFFSET + cursor_y*UNIT_SIZE, UNIT_SIZE, UNIT_SIZE, CURSOR);
	}
}


void show_logo() {
	print_image(160-32, 100-16, LOGO_HEIGHT, LOGO_WIDTH, logo);
}

uint8_t get_board_x(uint8_t offset) {return offset % BOARD_X;}
uint8_t get_board_y(uint8_t offset) {return (offset - get_board_x(offset))/BOARD_X;}
uint8_t get_board_offset(uint8_t x, uint8_t y) {return y*BOARD_X+x;}
