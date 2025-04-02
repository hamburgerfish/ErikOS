#include "port.h"

// read byte from specified port

uint8_t port_byte_in (uint16_t port) {
	uint8_t result;
	// inline assembler syntax has source and destination registers switched from nasm
	//
	// "=a" (result) set the C variable (result) to the value of register e'a'x
	// "d" (port) maps the C variable (port) into the e'd'x register
	// output and input are separated by :
	asm("in %%dx, %%al" : "=a" (result) : "d" (port));
	return result;
}

void port_byte_out (uint16_t port, uint8_t data) {
	// both registers are mapped to C variables, no return value
	
	asm("out %%al, %%dx" : : "a" (data), "d" (port));
}

uint16_t port_word_in (uint16_t port) {
	uint16_t result;
	asm("in %%dx, %%ax" : "=a" (result) : "d" (port));
	return result;
}

void port_word_out (uint16_t port, uint16_t data) {
	asm("out %%ax, %%dx" : : "a" (data), "d" (port));
}

