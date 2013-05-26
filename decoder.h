#ifndef __INJECTOR_DECODER_H__
#define __INJECTOR_DECODER_H__

#include <stdint.h>

/*
 * type of decoded instruction
 */
typedef enum 
{
  DI_NOT_WRITABLE = 0,
  DI_DST_REG, 
  DI_DST_MEM, 
  DI_DST_FLAG,
  DI_PUSH
} DIType;

/*
 * data structure for decoding information
 */

typedef struct __DecodingInfo
{
  uint8_t type;
  int8_t base;
  int8_t index;
  uint8_t scale;
  int32_t imm;
} _DecodingInfo;

void set_decoding_info(_DecodingInfo* info, uint8_t type, int8_t base, int8_t index, uint8_t scale, int32_t imm);
uint8_t decode_breakpoint(int pid, char* bp, _DecodingInfo* info);

#endif
