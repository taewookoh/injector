#include <stdio.h>
#include <stdint.h>

#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/user.h>

#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <ucontext.h>

static uint8_t __org_inst;

void setup_bp(int pid, unsigned long bp, unsigned long after_bp, void* after_bp_addr, void* org_inst_addr, unsigned target, void* target_addr)
{
  struct user_regs_struct regs;
  ptrace(PTRACE_GETREGS, pid, 0, &regs);
  printf("rip: %lx\n", regs.rip);

  uint64_t code = ptrace(PTRACE_PEEKTEXT, pid, bp, 0);
  uint64_t new_code = (code & 0xFFFFFFFFFFFFFF00L) | 0xcc;
  ptrace(PTRACE_POKEDATA, pid, bp, new_code);
  uint64_t confirm_code = ptrace(PTRACE_PEEKTEXT, pid, bp, 0);
  printf("%p, code: %lx -> %lx\n", bp, code, confirm_code);

  __org_inst = code & 0xff;

  // put org_inst
  uint64_t data = ptrace(PTRACE_PEEKDATA, pid, org_inst_addr, 0);
  uint64_t new_data = (data & 0xFFFFFFFFFFFFFF00L) | ( (uint64_t)__org_inst );
  ptrace(PTRACE_POKEDATA, pid, org_inst_addr, new_data);
  uint64_t confirm_data = ptrace(PTRACE_PEEKDATA, pid, org_inst_addr, 0);
  printf("%p, data: %lx -> %lx\n", org_inst_addr, data, confirm_data);

  // put after_bp
  ptrace(PTRACE_POKEDATA, pid, after_bp_addr, after_bp);

  // put target_dynamic_count
  ptrace(PTRACE_POKEDATA, pid, target_addr, target);
}

void suppress_bp(int pid, unsigned long bp)
{
  uint64_t code = ptrace(PTRACE_PEEKTEXT, pid, bp, 0);
  uint64_t new_code = (code & 0xFFFFFFFFFFFFFF00L) | ( (uint64_t)__org_inst );
  ptrace(PTRACE_POKETEXT, pid, bp, new_code);
  uint64_t confirm_code = ptrace(PTRACE_PEEKTEXT, pid, bp, 0);
  printf("suppress, code: %lx -> %lx\n", code, confirm_code);

  struct user_regs_struct regs;
  ptrace(PTRACE_GETREGS, pid, 0, &regs);
  printf("suppress, rip: %lx\n", regs.rip);

  // roll back one instruction
  regs.rip -= 1;
  ptrace(PTRACE_SETREGS, pid, 0, &regs);
}
