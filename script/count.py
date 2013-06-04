#!/usr/bin/python

import argparse
import sys
import subprocess
import random
import re
import math

NO_INJECTION = 100

injector = ''
prog = ''
timeout = -1
first_sample = 2**63-1
outdir = ''
max_process = 10
target = ''

def get_static_insts(prog):
  insts = []

  p = subprocess.Popen(['objdump', '-S', prog], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  out, err = p.communicate()

  lines = out.split('\n')
  for line in lines:
    if re.match( r'^  [0-9a-f]*:\t', line, re.M):
      part = line.partition(':\t')
      addr = part[0].lstrip()
      if '\t' in part[2]:   # valid disassmebled instruction exists
        insts.append(addr)

  return insts

def get_buffer_addrs(prog):
  p = subprocess.Popen(['objdump', '-D', prog], stdout=subprocess.PIPE)
  out, err = p.communicate()

  a = ''
  o = ''
  t = ''
  h = ''
  within_handler = False
  for line in out.split('\n'):
    if re.match( r'^[0-9a-f]+ <__after_bp>:', line, re.M):
      a = line.split()[0]
    if re.match( r'^[0-9a-f]+ <__org_inst>:', line, re.M):
      o = line.split()[0]
    if re.match( r'^[0-9a-f]+ <__target_dynamic_instance>:', line, re.M):
      t = line.split()[0]
    if re.match( r'^[0-9a-f]+ <install_sigtrap_handler>:', line, re.M):
      within_handler = True
    if within_handler and re.match( r'^  [0-9a-f]*:\t', line, re.M):
      insts = line.partition(':\t')[2]
      head = insts.split()[0]
      if ( head == 'c3' or head == 'cb' or head == 'c2' or head == 'ca' ):
        h = line.partition(':\t')[0].lstrip()
        within_handler = False
   
  return (a, o, t, h)

def reserve_process_queue(process, glog):
  while len(process) == max_process:
    sp_tuple = process[0]
    process.remove(sp_tuple)

    if (sp_tuple[0].poll() is None):
      process.append(sp_tuple)
    else:
      postprocess(sp_tuple, glog)

def cleanup(process, glog):
  while len(process) != 0:
    sp_tuple = process[0]
    process.remove(sp_tuple)

    if (sp_tuple[0].poll() is None):
      process.append(sp_tuple)
    else:
      postprocess(sp_tuple, glog)

def postprocess(sp_tuple, glog):
  returncode = sp_tuple[0].returncode
  breakpoint = sp_tuple[1]
  stdout = sp_tuple[3]
  stderr = sp_tuple[4]

  log = stdout.name
  out = stderr.name

  stdout.close()
  stderr.close()

  # remove out
  #p_rm = subprocess.Popen(['rm', '-f', out], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  #p_rm.wait() 

  count = 'NA'
  # parse logfile
  f_stdout = open(log, 'r')
  for line in f_stdout.readlines():
    if re.match(r'^__count:', line, re.M):
      count = line.rstrip('\n').split()[1]
  glog.write('bp %s count %s exitcode %d\n' % (breakpoint, count, returncode)) 

  return returncode != NO_INJECTION

def run(insts, after_bp_addr, org_inst_addr, target_addr, sigtrap_ret, target, glog):
  def get_args(inst, target_invocation):
    args = []
    args.append(injector)
    args.append('--type')
    args.append('a')
    args.append('--prog')
    args.append(prog)
    args.append('--breakpoint')
    args.append('0x%s' % inst)
    args.append('--target')
    args.append('%d' % target_invocation)
    args.append('--timeout')
    args.append('%d' % timeout)
    args.append('--after_bp_addr')
    args.append('0x%s' % after_bp_addr)
    args.append('--org_inst_addr')
    args.append('0x%s' % org_inst_addr)
    args.append('--target_addr')
    args.append('0x%s' % target_addr)
    args.append('--sigtrap_handler_ret')
    args.append('0x%s' % sigtrap_ret)
    return args

  # tuple of (Popen object, iteration, bptime)
  process = []

  index = 0
  target_invocation = 2**63-1

  if target != '':
    insts = [target]
  for inst in insts:
    reserve_process_queue(process, glog)

    # invoke process
    args = get_args(inst, target_invocation)
    outfile = outdir+'/output.%s' % (inst)
    logfile = outdir+'/log.%s' % (inst)

    f_stderr = open(outfile, 'w')
    f_stdout = open(logfile, 'w')
    sp = subprocess.Popen(args, stdout=f_stdout, stderr=f_stderr)
    if target != '':
      print args
    process.append( (sp, inst, target_invocation, f_stdout, f_stderr) )
  
  cleanup(process, glog)

if __name__ == '__main__':
  argparser = argparse.ArgumentParser(description='Injector driver')
  argparser.add_argument('-i', '--injector', help='Injector program', required=True)
  argparser.add_argument('-p', '--prog', help='Target program', required=True)
  argparser.add_argument('-t', '--timeout', type=float, help='Timeout', default=-1)
  argparser.add_argument('-o', '--outdir', help='Directory to put outputs', required=True)
  argparser.add_argument('-m', '--max_process', type=int, help='Maximum number of parallel processes', default=10)
  argparser.add_argument('-x', '--target', help='target breakpoint', default='')
  args = vars(argparser.parse_args())

  #initialize global variables 
  injector = args['injector']
  prog = args['prog']
  if timeout != -1 and timeout < 10: # minimum timeout
  	timeout = 10
  else:
    timeout = math.ceil(args['timeout'])
  outdir = args['outdir']
  max_process = args['max_process']
  target = args['target']

  insts = get_static_insts(prog)
  (after_bp_addr, org_inst_addr, target_addr, sigtrap_ret) = get_buffer_addrs(prog)

  glog = open(outdir+'/glog.txt', 'w')
  run(insts, after_bp_addr, org_inst_addr, target_addr, sigtrap_ret, target, glog)
  glog.close()
