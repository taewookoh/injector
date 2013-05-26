#include "decoder.h"

#include <stdio.h>
#include <stdint.h>

#include <sys/ptrace.h>

#include "distorm.h"
#include "mnemonics.h"

void set_decoding_info(_DecodingInfo* info, uint8_t type, int8_t base, int8_t index, uint8_t scale, int32_t imm)
{
  info->type = type;
  info->base = base;
  info->index = index;
  info->scale = scale;
  info->imm = imm;
}

/*
 * decode breakpoint-target x86 instruction. The first byte of the instruction
 * to be decoded needs to be store at the location 'addr'
 *
 * if decoded, return the length of the breakpointed instruction
 * else, return 0
 */
uint8_t decode_breakpoint(int pid, char* bp, _DecodingInfo* info)
{
  // constants 
  const int32_t OF = (1<<11);
  const int32_t DF = (1<<10);
  const int32_t IF = (1<<9);
  const int32_t TF = (1<<8);
  const int32_t SF = (1<<7);
  const int32_t ZF = (1<<6);
  const int32_t AF = (1<<4);
  const int32_t PF = (1<<2);
  const int32_t CF = (1);

  // any x86/x86-64 instruction cannot be longer than 15 bytes
  // TODO: possibility of seg fault if there are no following instructions
  uint8_t buf[16];
  uint64_t code[2];
  code[0] = ptrace(PTRACE_PEEKTEXT, pid, bp, 0);
  code[1] = ptrace(PTRACE_PEEKTEXT, pid, bp+8, 0);

  unsigned i = 0;
  for ( ; i < 16 ; i++)
    buf[i] = (code[i>>3] >> ( (i&0x7) << 3 )) & 0xff;

  // setup code info
  _CodeInfo ci;
  ci.code = buf;
  ci.codeLen = sizeof(buf);
  ci.codeOffset = 0;
  ci.dt = Decode64Bits;
  ci.features = DF_NONE;

  // decode one instruction with distorm3
  _DInst di;
  unsigned int di_count = 0;
  distorm_decompose(&ci, &di, 1, &di_count);

  // first, filter instructions  
  if ( META_GET_FC(di.meta) != FC_NONE && META_GET_FC(di.meta) != FC_CMOV )
    return 0;
  else if ( di.opcode == I_NOP || di.opcode == I_LEAVE || di.opcode == I_HLT )
    return 0;
  else if ( di.flags == FLAG_NOT_DECODABLE )
    return 0;

  // decode
  if ( di.flags & FLAG_DST_WR )
  {
    _Operand dst_op = di.ops[0];
    if (dst_op.type == O_REG)
    {
      // type 1
      set_decoding_info(info, DI_DST_REG, dst_op.index, -1, -1, 1);
    }
    else if (dst_op.type == O_SMEM)
    {
      // type 2
      int32_t imm = (di.dispSize == 0) ? -1 : (int64_t)di.disp;
      set_decoding_info(info, DI_DST_MEM, dst_op.index, -1, -1, imm);
    }
    else if (dst_op.type == O_MEM)
    {
      // type 2
      int8_t base = (di.base == R_NONE) ? -1 : di.base;
      int8_t index = (dst_op.index == R_NONE) ? -1 : dst_op.index;
      uint8_t scale = (di.scale == 0) ? -1 : di.scale;
      int32_t imm = (di.dispSize == 0) ? -1 : (int64_t)di.disp;
      set_decoding_info(info, DI_DST_MEM, base, index, scale, imm);
    }
  }
  else if (di.opcode == I_CMP)
  {
    // type 3: fix status reg
    int32_t flag = (SF|ZF|PF|CF|OF|AF);
    set_decoding_info(info, DI_DST_FLAG, -1, -1, -1, flag);
  }
  else if (di.opcode == I_TEST)
  {
    // type 3: fix status reg
    int32_t flag = (SF|ZF|PF|CF|OF);
    set_decoding_info(info, DI_DST_FLAG, -1, -1, -1, flag);
  }
  else if (di.opcode == I_PUSH)
  {
    // type 4: push
    set_decoding_info(info, DI_PUSH, -1, -1, -1, -1);
  }
  else if (di.opcode == I_CDQE)
  {
    // type 1: target eax
    set_decoding_info(info, DI_DST_REG, R_RAX, -1, -1, -1);
  }
  else
  {
    //check if dst_op is XMM registers indeed
    _Operand dst_op = di.ops[0];
    if (dst_op.type == O_REG && dst_op.index >= R_XMM0 && dst_op.index <= R_XMM15)
      set_decoding_info(info, DI_DST_REG, dst_op.index, -1, -1, -1);
    else
      return 0;
  }

  return di.size;
}
