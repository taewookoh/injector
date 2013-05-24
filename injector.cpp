#include "decoder.h"
#include "manage_bp.h"

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

#define NO_INJECTION 100

uint64_t bp = 0xFFFFFFFFFFFFFFFFL;
uint64_t target = 0xFFFFFFFFFFFFFFFFL;
enum _bp_type {NONE, ADDR, TIME} bp_type = NONE;
char* progname = NULL;
char* progargs = NULL;
int64_t timeout = -1;
char* after_bp_addr = NULL;
char* org_inst_addr = NULL;
char* target_addr = NULL;

static unsigned char org_inst;

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
  if ( target == 0xFFFFFFFFFFFFFFFFL )
  {
    fprintf(stderr, "delay (for TIME type) or dynamic target instance (for ADDR type) needs to be set with -d option");
    fprintf(stderr, " (-1 is an invalid value)\n");
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

void addr_type_handler(int pid)
{
  int status;
  int init = 0;

  while(1)
  {
    wait(&status);

    if (WIFEXITED(status))
    {
      printf("target exited\n");
      break;
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
          _DecodingInfo info;
          uint8_t size = decode_breakpoint(pid, (char*)bp, &info);
          if (size == 0)
          {
            fprintf(stderr, "unable to set breakpoint at instruction %p\n", (char*)bp);
            exit(NO_INJECTION);
          }
          printf("%lx, size: %u\n", bp, size);
          if (size)
            printf("-- %u %d %d %u %d\n", info.type, info.base, info.index, info.scale, info.imm);

          setup_bp(pid, bp, bp+size, after_bp_addr, org_inst_addr, target, target_addr);
          //ptrace(PTRACE_CONT,pid,0,0);
          ptrace(PTRACE_DETACH,pid,0,0);
        }
        else
        {
          // this should be called only after reattachment

          // suppress breakpoint
          suppress_bp(pid, bp);

          ptrace(PTRACE_CONT,pid,0,0);
        }
      }
      else
        ptrace(PTRACE_CONT,pid,0,WSTOPSIG(status));
    }
    else
    {
      fprintf(stderr, "signum: %d\n", WSTOPSIG(status));
      fprintf(stderr, "!WIFEXITED && !WIFSTOPPED");
      exit(1);
    }
  }
}

void time_type_handler(int pid)
{
  perror("not implemented yet\n");
  exit(1);
}

int main(int argc, char** argv)
{
  // parse command line
  parse_command_line_arg(argc, argv);

  int pid;
/*
  unsigned long bp = strtol(argv[1], NULL, 0);
  unsigned long after_bp = strtol(argv[2], NULL, 0);
  unsigned long after_bp_addr = strtol(argv[3], NULL, 0);
  unsigned long org_inst_addr = strtol(argv[4], NULL, 0);
*/
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
    if (bp_type == ADDR)
      addr_type_handler(pid);
    else // bp_type == TIME
      time_type_handler(pid);
  }
}
