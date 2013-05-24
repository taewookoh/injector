#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/user.h>

typedef enum { REGS, X87REGS, XMMREGS, UNSUPPORTED } RegType;

RegType get_regtype(uint8_t index);
unsigned long get_reg(struct user_regs_struct* regs, uint8_t index);
unsigned long set_reg(struct user_regs_struct* regs, uint8_t index, unsigned long val);
uint32_t get_fpreg(struct user_fpregs_struct* fpregs, uint8_t index, uint8_t byteoffet);
uint32_t set_fpreg(struct user_fpregs_struct* fpregs, uint8_t index, uint8_t byteoffset, uint32_t val);
unsigned get_bitwidth(uint8_t index);
unsigned is_high(uint8_t index);
const char* get_regname(uint8_t index);;
