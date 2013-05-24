#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ptrace.h>

#ifndef __USE_GNU
#define __USE_GNU
#endif

#include <ucontext.h>

unsigned __count = 0;

volatile void*    __bp;
volatile void*    __after_bp;
volatile char     __org_inst;
volatile char     __org_after_bp;
volatile char     __phase = 0;
volatile unsigned __target_dynamic_instance = 0;

static void sigtrap_handler(int sig, siginfo_t* siginfo, void* dummy)
{
  //fprintf(stderr, "sigtrap_handler\n");

  if (__phase == 0)
  {
    size_t pagesize = sysconf(_SC_PAGE_SIZE);
    size_t mask = 0xffffffffffffffff - (pagesize - 1);

    __count += 1;

    struct ucontext* u = (struct ucontext*)dummy;
    unsigned char* pc = (unsigned char *)u->uc_mcontext.gregs[REG_RIP];

    // remember original breakpoint
    __bp = (void*)(pc-1);

    // set codepage writable
    size_t codepage = ( (size_t)pc ) & mask;
    if(mprotect((void*) codepage, (size_t) pagesize, PROT_EXEC | PROT_READ | PROT_WRITE)==-1)
    {
      perror("mprotect");
      exit(1);
    }
    //printf("pc: %p, __org_inst: %x\n", pc, __org_inst);

    // retreive original instuction
    *(pc-1) = __org_inst;

    // set breakpoint at next instruction
    // to do that, first set the page read and writable
    size_t after_bp_page = ( (size_t)__after_bp) & mask;
    if(mprotect((void*) after_bp_page, (size_t) pagesize, PROT_EXEC | PROT_READ | PROT_WRITE)==-1)
    {
      perror("mprotect");
      exit(1);
    }

    // set sw breakpoint
    __org_after_bp = *( (char*)__after_bp);
    *( (char*)__after_bp ) = 0xcc;

    // fix PC
    u->uc_mcontext.gregs[REG_RIP] = (greg_t)(pc-1);

    // phase change
    __phase = 1;
  }
  else // __phase == 1
  {
    //printf("sig handler phase 1\n");

    // retreive original instruction
    *( (char*)__after_bp ) = __org_after_bp;

    // fix PC
    struct ucontext* u = (struct ucontext*)dummy;
    u->uc_mcontext.gregs[REG_RIP] -= 1;
    //printf("pc: %lx\n", u->uc_mcontext.gregs[REG_RIP]);

    // set breakpoint again
    *( (char*)__bp ) = 0xcc;

    // check count
    if (__count == (__target_dynamic_instance-1))
    {
      printf("reattach\n");
      ptrace(PTRACE_TRACEME,0,0,0);
    }

    __phase = 0;
  }
}

static void install_sigtrap_handler()
{
  struct sigaction o, r;
  r.sa_flags = SA_SIGINFO;
  sigemptyset( &r.sa_mask );
  r.sa_sigaction = &sigtrap_handler;
  sigaction( SIGTRAP, &r, &o);
}

