#include "decoder.h"
#include "bpmanager.h"
#include "flipper.h"

#include <stdio.h>
#include <stdlib.h>

#include <getopt.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/reg.h>

#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>

#include <vector>

/*
 * configuration 
 */

#define NO_INJECTION  100
#define FORCE_TIMEOUT 101

static uint64_t bp = 0xFFFFFFFFFFFFFFFFL;
static uint64_t target = 0;
static enum _bp_type {NONE, ADDR, TIME} bp_type = NONE;
static char* progname = NULL;
static char* progargs = NULL;
static int64_t timeout = -1;
static char* after_bp_addr = NULL;
static char* org_inst_addr = NULL;
static char* target_addr = NULL;

static int pid;

/*
 * SIGALRM handler for timeout
 */
static void sigalrm_handler(int sig, siginfo_t *siginfo, void *dummy)
{
  kill(pid, SIGKILL);
  exit(FORCE_TIMEOUT);
}

void parse_command_line_arg(int argc, char** argv)
{
  while(1)
  {
    static struct option long_options[] = 
    {
      {"type", required_argument, 0, 't'},
      {"breakpoint", required_argument, 0, 'b'},
      {"prog", required_argument, 0, 'p'},
      {"timeout", required_argument, 0, 'i'},
      {"target", required_argument, 0, 'g'},
      {"after_bp_addr", required_argument, 0, 'a'},
      {"org_inst_addr", required_argument, 0, 'o'},
      {"target_addr", required_argument, 0, 'r'},
      {0, 0, 0, 0}
    };

    int option_index = 0;
    int c = getopt_long(argc, argv, "t:b:d:p:a:o:c:", long_options, &option_index);

    // end of the options 
    if ( c == -1 )
      break;

    switch(c)
    {
      case 't':
        // type
        if ( !optarg || strlen(optarg) != 1 )
        {
          fprintf(stderr, "unsupported value for option 't', breakpoint type (-t) should be either 'a'(address) or 't'(time)\n");
          exit(1);
        }
        else if ( optarg[0] == 'a' )
          bp_type = ADDR;
        else if ( optarg[0] == 't' )
          bp_type = TIME;
        else
        {
          fprintf(stderr, "unsupported value for option 't', breakpoint type (-t) should be either 'a'(address) or 't'(time)\n");
          exit(1);
        }
        break;

      case 'b':
        // breakpoint
        if ( !optarg || strlen(optarg) < 2 )
        {
          fprintf(stderr, "target instruction needs to be given as hexadecimal number beginning with 0x\n", optarg);
          exit(1);
        }
        else if ( optarg[0] != '0' || ( optarg[1] != 'x' && optarg[1] != 'X' ) )
        {
          fprintf(stderr, "target instruction needs to be given as hexadecimal number beginning with 0x\n", optarg);
          exit(1);
        }
        bp = strtol(optarg, NULL, 0);
        break;

      case 'a':
        // after_bp_addr
        if ( !optarg || strlen(optarg) < 2 )
        {
          fprintf(stderr, "after_bp_addr needs to be given as hexadecimal number beginning with 0x\n", optarg);
          exit(1);
        }
        else if ( optarg[0] != '0' || ( optarg[1] != 'x' && optarg[1] != 'X' ) )
        {
          fprintf(stderr, "after_bp_addr needs to be given as hexadecimal number beginning with 0x\n", optarg);
          exit(1);
        }
        after_bp_addr = (char*)strtol(optarg, NULL, 0);
        break;

      case 'o':
        // org_inst_addr
        if ( !optarg || strlen(optarg) < 2 )
        {
          fprintf(stderr, "org_inst_addr needs to be given as hexadecimal number beginning with 0x\n", optarg);
          exit(1);
        }
        else if ( optarg[0] != '0' || ( optarg[1] != 'x' && optarg[1] != 'X' ) )
        {
          fprintf(stderr, "org_inst_addr needs to be given as hexadecimal number beginning with 0x\n", optarg);
          exit(1);
        }
        org_inst_addr = (char*)strtol(optarg, NULL, 0);
        break;

      case 'r':
        // target_addr
        if ( !optarg || strlen(optarg) < 2 )
        {
          fprintf(stderr, "target_addr needs to be given as hexadecimal number beginning with 0x\n", optarg);
          exit(1);
        }
        else if ( optarg[0] != '0' || ( optarg[1] != 'x' && optarg[1] != 'X' ) )
        {
          fprintf(stderr, "target_addr needs to be given as hexadecimal number beginning with 0x\n", optarg);
          exit(1);
        }
        target_addr = (char*)strtol(optarg, NULL, 0);
        break;

      case 'g':
        // target invocation count or target delay
        target = strtol(optarg, NULL, 10);
        break;

      case 'p':
        progname = strdup(optarg);
        break;

      case 'i':
        timeout = strtol(optarg, NULL, 10);
        break;

      default:
        fprintf(stderr, "unsupported option\n");
        exit(1);
    }
  }

  if ( progname == NULL)
  {
    fprintf(stderr, "target program name should be assigned with -p option\n");
    exit(1);
  }
  if ( bp_type == NONE )
  {
    fprintf(stderr, "breakpoint type needs to be set with -t option\n");
    exit(1);
  }
  if ( target == 0 )
  {
    fprintf(stderr, "delay (for TIME type) or dynamic target instance (for ADDR type) needs to be set with -d option");
    fprintf(stderr, " (should be a positive integer value)\n");
    exit(1);
  }
  if ( bp_type == ADDR && bp == 0xFFFFFFFFFFFFFFFFL )
  {
    fprintf(stderr, "for ADDR type breakpoint, breakpoint needs to be set\n");
    exit(1);
  }
  if ( bp_type == ADDR && after_bp_addr == NULL )
  {
    fprintf(stderr, "for ADDR type breakpoint, after_bp_addr needs to be set\n");
    exit(1);
  }
  if ( bp_type == ADDR && org_inst_addr == NULL )
  {
    fprintf(stderr, "for ADDR type breakpoint, org_inst_addr needs to be set\n");
    exit(1);
  }
  if ( bp_type == ADDR && target_addr == NULL )
  {
    fprintf(stderr, "for ADDR type breakpoint, target_addr needs to be set\n");
    exit(1);
  }
}

int addr_type_handler(int pid)
{
  int status;
  int init = 0;
  int hit = 0;
  _DecodingInfo info;

  while(1)
  {
    wait(&status);

    if (WIFEXITED(status))
    {
      printf("target exited\n");
      if (hit)
        return WEXITSTATUS(status);
      else
      {
        fprintf(stderr, "target terminated before hit the injection point\n");
        return NO_INJECTION;
      }
    }
    else if (WIFSTOPPED(status))
    {
      printf("signal: %s\n", strsignal(WSTOPSIG(status)));
      if ( WSTOPSIG(status) == SIGTRAP )
      {
        if (!init)
        {
          init = 1;

          // initialize information related to breakpoint
          uint8_t size = decode_breakpoint(pid, (char*)bp, &info);
          if (size == 0)
          {
            fprintf(stderr, "unable to set breakpoint at instruction %p\n", (char*)bp);
            kill(pid, SIGKILL);
            return NO_INJECTION;
          }
          printf("%lx, size: %u\n", bp, size);
          if (size)
            printf("-- %u %d %d %u %d\n", info.type, info.base, info.index, info.scale, info.imm);

          setup_bp(pid, bp, bp+size, after_bp_addr, org_inst_addr, target, target_addr);

          if (target == 1)
            ptrace(PTRACE_CONT,pid,0,0);
          else
            ptrace(PTRACE_DETACH,pid,0,0);
        }
        else
        {
          hit = 1;

          // this should be called only after reattachment

          // suppress breakpoint
          suppress_bp(pid, bp);

          // flip a destination
          stop_and_flip(pid, bp, info);

          ptrace(PTRACE_CONT,pid,0,0);
        }
      }
      else
        ptrace(PTRACE_CONT,pid,0,WSTOPSIG(status));
    }
    else
    {
      printf("signum: %d\n", WSTOPSIG(status));
      printf("!WIFEXITED && !WIFSTOPPED\n");
      kill(pid, SIGKILL);
      if (hit)
        return 1;
      else
      {
        fprintf(stderr, "target terminated before hit the injection point\n");
        return NO_INJECTION;
      }
    }
  }
}

void time_type_handler(int pid)
{
  perror("not implemented yet\n");
  kill(pid, SIGKILL);
  exit(1);
}

int main(int argc, char** argv)
{
  // initialize random seed
  srand(getpid());

  // register sigalrm handler
  struct sigaction original, replacement;
  replacement.sa_flags = SA_SIGINFO;
  sigemptyset( &replacement.sa_mask );
  replacement.sa_sigaction = &sigalrm_handler;
  sigaction( SIGALRM, &replacement, &original );

  // parse command line
  parse_command_line_arg(argc, argv);

  int exitcode = 0;

  if ( (pid = fork()) == -1 )
  {
    perror("fork");
    exit(1);
  }

  if (pid == 0)
  {
    ptrace(PTRACE_TRACEME,0,0,0);
    execv(progname, NULL);
    perror("execv");
    exit(1);
  }
  else
  {
    if (timeout != -1)
      alarm(timeout);

    if (bp_type == ADDR)
      exitcode = addr_type_handler(pid);
    else // bp_type == TIME
      time_type_handler(pid);
  }

  printf("exit with code %d\n", exitcode);
  exit(exitcode);
}
