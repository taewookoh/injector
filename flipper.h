#include "decoder.h"

#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/wait.h>

void stop_and_flip(int pid, uint64_t bp, _DecodingInfo info);
void flip_reg(int pid, _DecodingInfo info);
void flip_mem(int pid, _DecodingInfo info);
void flip_flag(int pid, _DecodingInfo info);
void flip_stack_top(int pid);
