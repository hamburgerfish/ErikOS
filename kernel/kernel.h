#ifndef KERNEL_H
#define KERNEL_H

#define KERNEL_2_OFFSET (uint8_t*) 0x8000
#define KERNEL_1_END (uint8_t*) 0x7d00 //0x7c00

void user_input(char *input);

#endif
