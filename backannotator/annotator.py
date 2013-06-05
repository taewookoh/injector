#!/usr/bin/python

import argparse
import sys

class _Inst(object):
  def __init__(self):
    self.total = 0
    self.mask = 0
    self.crash = 0
    self.timeout = 0
    self.sdc = 0
    self.detect = 0

def parse_dwarfdump(dwarfdump):
  # insts[addr] = _Inst instance
  insts = {}

  #locations[source][line] = [ instr0, instr1, ... ]
  locations = {}

  f = open(dwarfdump, 'r')
  for line in f.readlines():
    line = line.rstrip('\n')
    l = line.split(', ')
    
    addr = l[0]
    source = l[1]
    line = int(l[2])

    insts[addr] = _Inst()

    if source in locations:
      if line in locations[source]:
        locations[source][line].append(addr)
      else:
        locations[source][line] = [addr]
    else:
      m = {}
      m[line] = [addr]
      locations[source] = m

  f.close()
  return (insts, locations)

def parse_log(log, insts):
  def get_bp(line):
    return '0x'+line.partition('bp ')[2].partition(' ')[0]

  def get_exitcode(line):
    return line.partition('exitcode ')[2].partition(' ')[0]

  def get_cmp(line):
    return line.partition('cmpresult ')[2].partition(' ')[0]

  f = open(log, 'r')
  for line in f.readlines():
    line = line.rstrip('\n')
    bp = get_bp(line)
    
    if not bp in insts:
      continue 

    exitcode = get_exitcode(line)
    # 100 = NO_INJECTION, 104 = UNABLE_TO_INJECT
    if exitcode == '100' or exitcode == '104':
      continue

    inst = insts[bp]
    inst.total += 1

    cmp = get_cmp(line)
    # 101 = timeout, 102 = detected, 0 with cmp 0 = masked, 0 with cmp 1 = sdc, crashed else
    if exitcode == '101':
      inst.timeout += 1
    elif exitcode == '102':
      inst.detect += 1
    elif exitcode == '0':
      if cmp == '0': 
        inst.mask += 1
      elif cmp == '1': 
        inst.sdc += 1
      else:
        assert False, "unexpected log file parsing result"
    else:
      inst.crash += 1

  f.close()

def append_annotation(addrlist, insts, outlist, line):
  for addr in addrlist:
    if not addr in insts:
      continue 
    inst = insts[addr]

    total = inst.total
    if (total == 0):
      continue

    m = inst.mask
    m_perc = m / float(total)
    c = inst.crash
    c_perc = c / float(total)
    t = inst.timeout
    t_perc = t / float(total)
    s = inst.sdc
    s_perc = s / float(total)
    d = inst.detect
    d_perc = d / float(total)
    s = '// [FAULTINJECTION] line %d, total %d mask %d(%.2f) crash %d(%.2f) timeout %d(%.2f) sdc %d(%.2f) detect %d(%2.f)\n' % (line, total, m, m_perc, c, c_perc, t, t_perc, s, s_perc, d, d_perc)
    outlist.append(s)

def annotate(locations, insts, outdir):
  for source in locations:
    outlist = []

    filename = source.rpartition('/')[2]
    if filename == 'lwc.inc':
      continue

    m = locations[source]
    inlist = []
    try:
      f = open(source, 'r')
      inlist = f.readlines()
      f.close()
    except IOError as e:
      print 'cannot open', source
      sys.exit(1)
   
    index = 1
    for linenum in sorted(m.iterkeys()):
      while index < linenum:
        outlist.append( inlist[index-1] )
        index += 1

      # add annatation
      append_annotation(m[linenum], insts, outlist, linenum)
      
    while index <= len(inlist):
      outlist.append( inlist[index-1] )
      index += 1
   
    # dump output
    f = open(outdir+'/'+filename+'.anno', 'w')
    for line in outlist:
      f.write(line)
    f.close()

if __name__=='__main__':
  argparser = argparse.ArgumentParser(description='Back annotator')
  argparser.add_argument('-l', '--log', help='Log file summarizing experimental results', required=True)
  argparser.add_argument('-d', '--dwarfdump', help='Dwarfdump result, generated by dwarfparser', required=True)
  argparser.add_argument('-o', '--outdir', help='Directory to put annotated file.', required=True)
  args = vars(argparser.parse_args())

  (insts, locations) = parse_dwarfdump(args['dwarfdump'])
  parse_log(args['log'], insts)
  annotate(locations, insts, args['outdir'])
