#!/usr/bin/python

import argparse
import sys
import subprocess
import random
import re
import math

NO_INJECTION = 100
NO_INJECTION_CRASH = 103
UNABLE_TO_INJECT = 104

injector = ''
prog = ''
cmp = ''
ref = ''
timeout = -1
sampling_interval = 10000
first_sample = 0
outdir = ''

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

def postprocess(sp_tuple, glog):
  returncode = sp_tuple[0].returncode
  breakpoint = sp_tuple[1]
  target_invocation = sp_tuple[2]
  stdout = sp_tuple[3]
  stderr = sp_tuple[4]

  log = stdout.name
  out = stderr.name

  stdout.close()
  stderr.close()

  # compare output
  cmpout = open( outdir+'/cmp.%s.%d' % (breakpoint, target_invocation), 'w' )
  p_cmp = subprocess.Popen([cmp, out, ref], stdout=cmpout, stderr=subprocess.PIPE)
  cmpresult = p_cmp.wait()
  cmpout.close()

  # remove out
  p_rm = subprocess.Popen(['rm', '-f', out], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  p_rm.wait() 

  injectionlog = '\n'
  # parse logfile
  f_stdout = open(log, 'r')
  for line in f_stdout.readlines():
    if re.match(r'^\[injection,.+\]$', line, re.M):
      injectionlog = line
  glog.write('bp %s invocation %d exitcode %d cmpresult %d log %s' % (breakpoint, target_invocation, returncode, cmpresult, injectionlog)) 

  return returncode != NO_INJECTION and returncode != UNABLE_TO_INJECT

def run(insts, after_bp_addr, org_inst_addr, target_addr, sigtrap_ret, glog):
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
  index = 0
  sample_count = 0
  target_invocation = first_sample
  while index < len(insts):
    inst = insts[index]

    # invoke process
    args = get_args(inst, target_invocation)
    outfile = outdir+'/output.%s.%d' % (inst,target_invocation)
    logfile = outdir+'/log.%s.%d' % (inst,target_invocation)

    f_stderr = open(outfile, 'w')
    f_stdout = open(logfile, 'w')
    sp = subprocess.Popen(args, stdout=f_stdout, stderr=f_stderr)

    sp.wait()
    injected = postprocess( (sp, inst, target_invocation, f_stdout, f_stderr), glog )
  
    if injected:
      target_invocation += sampling_interval 
      sample_count += 1
    else:
      index += 1
      sample_count = 0
      target_invocation = first_sample

def summarize(glog, summary):
  pcs = []

  masked = 0      # exitcode == 0, cmp == 0
  sdc = 0         # exitcode == 0, cmp == 1
  skipped = 0     # exitcode == 100
  timeout= 0      # exitcode == 100
  detected = 0    # exitcode == 102
  crashed = 0     # else
  total = 0

  for line in glog.readlines():
    total += 1
    exitcode = line.partition('exitcode ')[2].partition(' ')[0]
    pc = line.partition('bp ')[2].partition(' ')[0]
    pcs.append(pc)
    cmp = line.partition('cmpresult ')[2].partition(' ')[0]

    if (exitcode == '0'):
      if (cmp == '0'):
        masked += 1
      elif (cmp == '1'):
        sdc += 1
      else:
        summary.write('unsupported cmpresult %s appeared, line %s' % (cmp, line))
        return
    elif (exitcode == '100'):
      skipped += 1
    elif (exitcode == '101'):
      timeout += 1
    elif (exitcode == '102'):
      detected += 1
    else:
      crashed += 1

  total_org = total
  total -= skipped
  summary.write('total: %d (%d - %d)\n' % (total, total_org, skipped) )

  masked_perc = '%.2f' % (float(masked)/float(total)*100)
  summary.write('masked: %d(%s)\n' % ( masked, masked_perc ))

  sdc_perc = '%.2f' % (float(sdc)/float(total)*100)
  summary.write('sdc: %d(%s)\n' % ( sdc, sdc_perc ))

  #skipped_perc = '%.2f' % (float(skipped)/float(total)*100)
  #summary.write('skipped: %d(%s)\n' % ( skipped, skipped_perc ))

  timeout_perc = '%.2f' % (float(timeout)/float(total)*100)
  summary.write('timeout: %d(%s)\n' % ( timeout, timeout_perc ))

  detected_perc = '%.2f' % (float(detected)/float(total)*100)
  summary.write('detected: %d(%s)\n' % ( detected, detected_perc ))

  crashed_perc = '%.2f' % (float(crashed)/float(total)*100)
  summary.write('crashed: %d(%s)\n' % ( crashed, crashed_perc ))

  pcs_set = set(pcs)
  summary.write('static breakpoint: %d\n' % len( pcs_set ))

  for bp in pcs_set:
    summary.write('  %s : %d\n' % (bp, pcs.count(bp)))

if __name__ == '__main__':
  argparser = argparse.ArgumentParser(description='Injector driver')
  argparser.add_argument('-i', '--injector', help='Injector program', required=True)
  argparser.add_argument('-p', '--prog', help='Target program', required=True)
  argparser.add_argument('-c', '--cmp', help='Program to compare outputs. Exit code should be 0 if outputs are same, non-zero otherwise', required=True)
  argparser.add_argument('-r', '--ref', help='Reference output to check correctness', required=True)
  argparser.add_argument('-t', '--timeout', type=float, help='Timeout', default=-1)
  argparser.add_argument('-s', '--sampling_interval', type=int, help='Sampling interval', default=10000)
  argparser.add_argument('-f', '--first_sample', type=int, help='Invocation count of first sample for each breakpoint', default=1)
  argparser.add_argument('-o', '--outdir', help='Directory to put outputs', required=True)
  args = vars(argparser.parse_args())

  #initialize global variables 
  injector = args['injector']
  prog = args['prog']
  cmp = args['cmp']
  ref = args['ref']
  if timeout != -1 and timeout < 10: # minimum timeout
  	timeout = 10
  else:
    timeout = math.ceil(args['timeout'])
  sampling_interval = args['sampling_interval']
  first_sample = args['first_sample']
  outdir = args['outdir']

  insts = get_static_insts(prog)
  (after_bp_addr, org_inst_addr, target_addr, sigtrap_ret) = get_buffer_addrs(prog)

  glog = open(outdir+'/glog.txt', 'w')
  run(insts, after_bp_addr, org_inst_addr, target_addr, sigtrap_ret, glog)
  glog.close()

  glog = open(outdir+'/glog.txt', 'r')
  summary = open(outdir+'/summary.txt', 'w')
  summarize(glog, summary)
  summary.close()
  glog.close()

