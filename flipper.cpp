#include "flipper.h"
#include "regmap.h"

#include <signal.h>

void stop_and_flip(int pid, uint64_t bp, _DecodingInfo info)
{
  // check rip
  struct user_regs_struct regs;
  ptrace(PTRACE_GETREGS, pid, 0, &regs);
  unsigned long rip = regs.rip;

  if (rip != bp)
  {
    fprintf(stderr, "stop_and_flip, rip is different from bp\n");
    kill(pid, SIGKILL);
    exit(1);
  }

#ifdef DEBUG
  printf("[%lx] single stepping\n", rip);
#endif

  // single stepping
  long res;
  int status;
  if ((res = ptrace(PTRACE_SINGLESTEP, pid, 0, 0)) < 0)
  {
    perror("ptrace singlestep\n");
    kill(pid, SIGKILL);
    exit(EXIT_FAILURE);
  }
  res = wait(&status);

  // flip the destination
  if (info.type == DI_DST_REG)
    flip_reg(pid, info);
  else if (info.type == DI_DST_MEM)
    flip_mem(pid, info);
  else if (info.type == DI_DST_FLAG)
    flip_flag(pid, info);
  else if (info.type == DI_PUSH)
    flip_stack_top(pid);
  else
  {
    perror("unsupported decoded instruction type\n");
    kill(pid, SIGKILL);
    exit(EXIT_FAILURE);
  }
}

void flip_reg(int pid, _DecodingInfo info)
{
  if ( get_regtype(info.base) == REGS )
  {
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, 0, &regs);

    unsigned long val = get_reg(&regs, info.base);
    unsigned bitwidth = get_bitwidth(info.base);
    unsigned togglebit = rand() % bitwidth;
    if ( is_high(info.base) )
      togglebit += 8;
    unsigned long newval = val ^ (1 << togglebit);
    set_reg(&regs, info.base, newval);

    ptrace(PTRACE_SETREGS, pid, 0, &regs);

    // verify
    ptrace(PTRACE_GETREGS, pid, 0, &regs);
    newval = get_reg(&regs, info.base);
    printf("injection, flip_reg, %s, %lx->%lx (bit %u)\n", get_regname(info.base), val, newval, togglebit);
  }
  else if ( get_regtype(info.base) == X87REGS )
  {
    struct user_fpregs_struct regs;
    ptrace(PTRACE_GETFPREGS, pid, 0, &regs);

    unsigned togglebit = rand() % 64;
    uint32_t val = get_fpreg(&regs, info.base, togglebit / 32);
    uint32_t newval = val ^ ( 1 << (togglebit % 32) );
    set_fpreg(&regs, info.base, togglebit / 32, newval);

    ptrace(PTRACE_SETFPREGS, pid, 0, &regs);

    // verify
    ptrace(PTRACE_GETFPREGS, pid, 0, &regs);
    newval = get_fpreg(&regs, info.base, togglebit / 32);
    printf("injection, flip_reg, %s, %x->%x (bit %d)\n", get_regname(info.base), val, newval, togglebit);
  }
  else if ( get_regtype(info.base) == XMMREGS )
  {
    struct user_fpregs_struct regs;
    ptrace(PTRACE_GETFPREGS, pid, 0, &regs);

    unsigned togglebit = rand() % 128;
    uint32_t val = get_fpreg(&regs, info.base, togglebit / 32);
    uint32_t newval = val ^ ( 1 << (togglebit % 32) );
    set_fpreg(&regs, info.base, togglebit / 32, newval);

    ptrace(PTRACE_SETFPREGS, pid, 0, &regs);

    // verify
    ptrace(PTRACE_GETFPREGS, pid, 0, &regs);
    newval = get_fpreg(&regs, info.base, togglebit / 32);
    printf("injection, flip_reg, %s, %x->%x (bit %d)\n", get_regname(info.base), val, newval, togglebit);
  }
  else
  {
    perror("Unsupported register type\n");
    kill(pid, SIGKILL);
    exit(EXIT_FAILURE);
  }
}

void flip_mem(int pid, _DecodingInfo info)
{
  struct user_regs_struct regs;
  ptrace(PTRACE_GETREGS, pid, 0, &regs);

  unsigned long base = 0;
  if (info.base != -1)
    base = get_reg(&regs, info.base);

  unsigned long index = 0;
  if (info.index != -1)
    index = get_reg(&regs, info.index) * info.scale;

  uint64_t addr = base + index + info.imm;
  uint64_t data = ptrace(PTRACE_PEEKDATA, pid, addr, 0);

  unsigned togglebit = rand() % 64;
  uint64_t newdata = data ^ (1 << togglebit);

  ptrace(PTRACE_POKEDATA, pid, addr, newdata);

  // verify
  newdata = ptrace(PTRACE_PEEKDATA, pid, addr, 0);
  printf("injection, flip_mem, %lx, %lx->%lx (bit %d)\n", addr, data, newdata, togglebit);
}

void flip_flag(int pid, _DecodingInfo info)
{
  struct user_regs_struct regs;
  ptrace(PTRACE_GETREGS, pid, 0, &regs);
  unsigned long val = regs.eflags;

  unsigned togglebit = rand() % 16;
  unsigned togglemask = (1 << togglebit);
  while ( !(togglemask & info.imm))
  {
    togglebit = rand() % 16;
    togglemask = (1 << togglebit);
  }

  unsigned long newval = val ^ togglemask;
  regs.eflags = newval;
    
  ptrace(PTRACE_SETREGS, pid, 0, &regs);

  // verify
  ptrace(PTRACE_GETREGS, pid, 0, &regs);
  newval = regs.eflags;
  printf("injection, flip_flag, EFLAGS, %lx->%lx (bit %d)\n", val, newval, togglebit);
}

void flip_stack_top(int pid)
{
  struct user_regs_struct regs;
  ptrace(PTRACE_GETREGS, pid, 0, &regs);

  uint64_t addr = regs.rsp;
  uint64_t data = ptrace(PTRACE_PEEKDATA, pid, addr, 0);

  unsigned togglebit = rand() % 64;
  uint64_t newdata = data ^ (1 << togglebit);

  ptrace(PTRACE_POKEDATA, pid, addr, newdata);

  // verify
  newdata = ptrace(PTRACE_PEEKDATA, pid, addr, 0);
  printf("injection, flip_stack_top, %lx, %lx->%lx (bit %d)\n", addr, data, newdata, togglebit);
}
