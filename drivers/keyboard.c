#include "keyboard.h"
#include "../cpu/port.h"
#include "../cpu/isr.h"
#include "screen.h"
#include "../libc/string.h"
#include "../libc/function.h"
#include "../kernel/kernel.h"
#include "../minesweeper/minesweeper.h"
#include <stdint.h>

#define ENTER 0x1C
#define UP 0x11
#define LEFT 0x1E
#define DOWN 0x1F
#define RIGHT 0x20
#define FLAG_MODE 0x21

#define SC_MAX 57
const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6",
	"7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E",
	"R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl",
	"A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`",
	"LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".",
	"/", "RShift", "Keypad *", "LAlt", "Spacebar"};
const char sc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',
	'7', '8', '9', '0', '-', '=', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y',
	'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G',
	'H', 'J', 'K', 'L', ';', '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' '};

static void keyboard_callback(registers_t *regs) {
    /* The PIC leaves us the scancode in port 0x60 */
    uint8_t scancode = port_byte_in(0x60);
    
    if (scancode > SC_MAX) return;
    if (scancode == ENTER) {
	    reveal();
    } else if (scancode == UP || scancode == LEFT || scancode == DOWN || scancode == RIGHT) {
	    move_cursor(scancode);
    }
    else if (scancode == FLAG_MODE) {
	    switch_mode();
    }

    UNUSED(regs);
}

void init_keyboard() {
   register_interrupt_handler(IRQ1, keyboard_callback); 
}


