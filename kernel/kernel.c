#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "kernel.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include "../cpu/timer.h"
#include "../minesweeper/minesweeper.h"
#include <stdint.h>


void kernel_main() {
	isr_install();
	irq_install();

	clear_screen();
	show_logo();
	sleep(5);

	init_board();
	init_counter();
}

void user_input(char *input) {
	if (strcmp(input, "END") == 0) {
		asm volatile("hlt");
	}
	if (strcmp(input, "MINESWEEPER") == 0) {
		clear_screen();
	}
}
