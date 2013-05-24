#include <stdint.h>

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
