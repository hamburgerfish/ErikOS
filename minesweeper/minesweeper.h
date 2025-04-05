#ifndef MINESWEEPER
#define MINESWEEPER


#include <stdint.h>

#define UNIT_SIZE 16
#define BOARD_X 20
#define BOARD_Y 10
#define HEADER_Y_OFFSET 12
#define BOARD_Y_OFFSET 40

#define TOTAL_MINE_COUNT 35



typedef struct {
	uint8_t hunds;
	uint8_t tens;
	uint8_t ones;
} counter;

uint32_t rng();
void init_board();
void start();
void init_counter();
void switch_mode();
void move_cursor(uint8_t dir);
void reveal();
void show_logo();

#endif
