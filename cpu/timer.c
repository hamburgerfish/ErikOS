#include "timer.h"
#include "../libc/function.h"
#include "isr.h"
#include "port.h"

uint32_t tick = 0;

void sleep(uint8_t seconds) {
	uint32_t cpu_time = 100000000 * seconds;
	for (uint32_t i=0; i < cpu_time; i++);
}

static void timer_callback(registers_t *regs) {
	tick++;
	UNUSED(regs);
}

void init_timer(uint32_t freq) {
	register_interrupt_handler(IRQ0, timer_callback);

	//hardware clock is at 1193180 Hz
	uint32_t divisor = 1193180 / freq;
	uint8_t low = (uint8_t)(divisor & 0xFF);
	uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);

	port_byte_out(0x43, 0x36); //command port
	port_byte_out(0x40, low);
	port_byte_out(0x40, high);
}
