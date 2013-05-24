#include "regmap.h"

RegType get_regtype(uint8_t index)
{
	if ( index <= 67 )
		return REGS;
	if ( index >= 75 && index <= 82 )
		return X87REGS;
	if ( index >= 91 && index <= 106 )
		return XMMREGS;
	return UNSUPPORTED;
}

unsigned long get_reg(struct user_regs_struct* regs, uint8_t index)
{
	switch(index)
	{
		case 0:
			//RAX
			return regs->rax;
		case 1:
			//RCX
			return regs->rcx;
		case 2:
			//RDX
			return regs->rdx;
		case 3:
			//RBX
			return regs->rbx;
		case 4:
			//RSP
			return regs->rsp;
		case 5:
			//RBP
			return regs->rbp;
		case 6:
			//RSI
			return regs->rsi;
		case 7:
			//RDI
			return regs->rdi;
		case 8:
			//R8
			return regs->r8;
		case 9:
			//R9
			return regs->r9;
		case 10:
			//R10
			return regs->r10;
		case 11:
			//R11
			return regs->r11;
		case 12:
			//R12
			return regs->r12;
		case 13:
			//R13
			return regs->r13;
		case 14:
			//R14
			return regs->r14;
		case 15:
			//R15
			return regs->r15;
		case 16:
			//EAX
			return regs->rax;
		case 17:
			//ECX
			return regs->rcx;
		case 18:
			//EDX
			return regs->rdx;
		case 19:
			//EBX
			return regs->rbx;
		case 20:
			//ESP
			return regs->rsp;
		case 21:
			//EBP
			return regs->rbp;
		case 22:
			//ESI
			return regs->rsi;
		case 23:
			//EDI
			return regs->rdi;
		case 24:
			//R8D
			return regs->r8;
		case 25:
			//R9D
			return regs->r9;
		case 26:
			//R10D
			return regs->r10;
		case 27:
			//R11D
			return regs->r11;
		case 28:
			//R12D
			return regs->r12;
		case 29:
			//R13D
			return regs->r13;
		case 30:
			//R14D
			return regs->r14;
		case 31:
			//R15D
			return regs->r15;
		case 32:
			//AX
			return regs->rax;
		case 33:
			//CX
			return regs->rcx;
		case 34:
			//DX
			return regs->rdx;
		case 35:
			//BX
			return regs->rbx;
		case 36:
			//SP
			return regs->rsp;
		case 37:
			//BP
			return regs->rbp;
		case 38:
			//SI
			return regs->rsi;
		case 39:
			//DI
			return regs->rdi;
		case 40:
			//R8W
			return regs->r8;
		case 41:
			//R9W
			return regs->r9;
		case 42:
			//R10W
			return regs->r10;
		case 43:
			//R11W
			return regs->r11;
		case 44:
			//R12W
			return regs->r12;
		case 45:
			//R13W
			return regs->r13;
		case 46:
			//R14W
			return regs->r14;
		case 47:
			//R15W
			return regs->r15;
		case 48:
			//AL
			return regs->rax;
		case 49:
			//CL
			return regs->rcx;
		case 50:
			//DL
			return regs->rdx;
		case 51:
			//BL
			return regs->rbx;
		case 52:
			//AH
			return regs->rax;
		case 53:
			//CH
			return regs->rcx;
		case 54:
			//DH
			return regs->rdx;
		case 55:
			//BH
			return regs->rbx;
		case 56:
			//R8B
			return regs->r8;
		case 57:
			//R9B
			return regs->r9;
		case 58:
			//R10B
			return regs->r10;
		case 59:
			//R11B
			return regs->r11;
		case 60:
			//R12B
			return regs->r12;
		case 61:
			//R13B
			return regs->r13;
		case 62:
			//R14B
			return regs->r14;
		case 63:
			//R15B
			return regs->r15;
		case 64:
			//SPL
			return regs->rsp;
		case 65:
			//BPL
			return regs->rbp;
		case 66:
			//SIL
			return regs->rsi;
		case 67:
			//DIL
			return regs->rdi;
		default:
			fprintf(stderr, "get_reg, unsupported register index, %d\n", index);
			exit(EXIT_FAILURE);
	}
}

unsigned long set_reg(struct user_regs_struct* regs, uint8_t index, unsigned long val)
{
	switch(index)
	{
		case 0:
			//RAX
			regs->rax = val;
			break;
		case 1:
			//RCX
			regs->rcx = val;
			break;
		case 2:
			//RDX
			regs->rdx = val;
			break;
		case 3:
			//RBX
			regs->rbx = val;
			break;
		case 4:
			//RSP
			regs->rsp = val;
			break;
		case 5:
			//RBP
			regs->rbp = val;
			break;
		case 6:
			//RSI
			regs->rsi = val;
			break;
		case 7:
			//RDI
			regs->rdi = val;
			break;
		case 8:
			//R8
			regs->r8 = val;
			break;
		case 9:
			//R9
			regs->r9 = val;
			break;
		case 10:
			//R10
			regs->r10 = val;
			break;
		case 11:
			//R11
			regs->r11 = val;
			break;
		case 12:
			//R12
			regs->r12 = val;
			break;
		case 13:
			//R13
			regs->r13 = val;
			break;
		case 14:
			//R14
			regs->r14 = val;
			break;
		case 15:
			//R15
			regs->r15 = val;
			break;
		case 16:
			//EAX
			regs->rax = val;
			break;
		case 17:
			//ECX
			regs->rcx = val;
			break;
		case 18:
			//EDX
			regs->rdx = val;
			break;
		case 19:
			//EBX
			regs->rbx = val;
			break;
		case 20:
			//ESP
			regs->rsp = val;
			break;
		case 21:
			//EBP
			regs->rbp = val;
			break;
		case 22:
			//ESI
			regs->rsi = val;
			break;
		case 23:
			//EDI
			regs->rdi = val;
			break;
		case 24:
			//R8D
			regs->r8 = val;
			break;
		case 25:
			//R9D
			regs->r9 = val;
			break;
		case 26:
			//R10D
			regs->r10 = val;
			break;
		case 27:
			//R11D
			regs->r11 = val;
			break;
		case 28:
			//R12D
			regs->r12 = val;
			break;
		case 29:
			//R13D
			regs->r13 = val;
			break;
		case 30:
			//R14D
			regs->r14 = val;
			break;
		case 31:
			//R15D
			regs->r15 = val;
			break;
		case 32:
			//AX
			regs->rax = val;
			break;
		case 33:
			//CX
			regs->rcx = val;
			break;
		case 34:
			//DX
			regs->rdx = val;
			break;
		case 35:
			//BX
			regs->rbx = val;
			break;
		case 36:
			//SP
			regs->rsp = val;
			break;
		case 37:
			//BP
			regs->rbp = val;
			break;
		case 38:
			//SI
			regs->rsi = val;
			break;
		case 39:
			//DI
			regs->rdi = val;
			break;
		case 40:
			//R8W
			regs->r8 = val;
			break;
		case 41:
			//R9W
			regs->r9 = val;
			break;
		case 42:
			//R10W
			regs->r10 = val;
			break;
		case 43:
			//R11W
			regs->r11 = val;
			break;
		case 44:
			//R12W
			regs->r12 = val;
			break;
		case 45:
			//R13W
			regs->r13 = val;
			break;
		case 46:
			//R14W
			regs->r14 = val;
			break;
		case 47:
			//R15W
			regs->r15 = val;
			break;
		case 48:
			//AL
			regs->rax = val;
			break;
		case 49:
			//CL
			regs->rcx = val;
			break;
		case 50:
			//DL
			regs->rdx = val;
			break;
		case 51:
			//BL
			regs->rbx = val;
			break;
		case 52:
			//AH
			regs->rax = val;
			break;
		case 53:
			//CH
			regs->rcx = val;
			break;
		case 54:
			//DH
			regs->rdx = val;
			break;
		case 55:
			//BH
			regs->rbx = val;
			break;
		case 56:
			//R8B
			regs->r8 = val;
			break;
		case 57:
			//R9B
			regs->r9 = val;
			break;
		case 58:
			//R10B
			regs->r10 = val;
			break;
		case 59:
			//R11B
			regs->r11 = val;
			break;
		case 60:
			//R12B
			regs->r12 = val;
			break;
		case 61:
			//R13B
			regs->r13 = val;
			break;
		case 62:
			//R14B
			regs->r14 = val;
			break;
		case 63:
			//R15B
			regs->r15 = val;
			break;
		case 64:
			//SPL
			regs->rsp = val;
			break;
		case 65:
			//BPL
			regs->rbp = val;
			break;
		case 66:
			//SIL
			regs->rsi = val;
			break;
		case 67:
			//DIL
			regs->rdi = val;
			break;
		default:
			fprintf(stderr, "set_reg, unsupported register index\n");
			exit(EXIT_FAILURE);
	}
}

uint32_t get_fpreg(struct user_fpregs_struct* fpregs, uint8_t index, uint8_t byteoffset)
{
	switch(index)
	{
		case 75:
			//ST0
			return fpregs->st_space[0+byteoffset];
		case 76:
			//ST1
			return fpregs->st_space[4+byteoffset];
		case 77:
			//ST2
			return fpregs->st_space[8+byteoffset];
		case 78:
			//ST3
			return fpregs->st_space[12+byteoffset];
		case 79:
			//ST4
			return fpregs->st_space[16+byteoffset];
		case 80:
			//ST5
			return fpregs->st_space[20+byteoffset];
		case 81:
			//ST6
			return fpregs->st_space[24+byteoffset];
		case 82:
			//ST7
			return fpregs->st_space[28+byteoffset];
		case 91:
			//XMM0
			return fpregs->xmm_space[0+byteoffset];
		case 92:
			//XMM1
			return fpregs->xmm_space[4+byteoffset];
		case 93:
			//XMM2
			return fpregs->xmm_space[8+byteoffset];
		case 94:
			//XMM3
			return fpregs->xmm_space[12+byteoffset];
		case 95:
			//XMM4
			return fpregs->xmm_space[16+byteoffset];
		case 96:
			//XMM5
			return fpregs->xmm_space[20+byteoffset];
		case 97:
			//XMM6
			return fpregs->xmm_space[24+byteoffset];
		case 98:
			//XMM7
			return fpregs->xmm_space[28+byteoffset];
		case 99:
			//XMM8
			return fpregs->xmm_space[32+byteoffset];
		case 100:
			//XMM9
			return fpregs->xmm_space[36+byteoffset];
		case 101:
			//XMM10
			return fpregs->xmm_space[40+byteoffset];
		case 102:
			//XMM11
			return fpregs->xmm_space[44+byteoffset];
		case 103:
			//XMM12
			return fpregs->xmm_space[48+byteoffset];
		case 104:
			//XMM13
			return fpregs->xmm_space[52+byteoffset];
		case 105:
			//XMM14
			return fpregs->xmm_space[56+byteoffset];
		case 106:
			//XMM15
			return fpregs->xmm_space[60+byteoffset];
		default:
			fprintf(stderr, "get_fpreg, unsupported register index\n");
			exit(EXIT_FAILURE);
	}
}

uint32_t set_fpreg(struct user_fpregs_struct* fpregs, uint8_t index, uint8_t byteoffset, uint32_t val)
{
	switch(index)
	{
		case 75:
			//ST0
			fpregs->st_space[0+byteoffset] = val;
			break;
		case 76:
			//ST1
			fpregs->st_space[4+byteoffset] = val;
			break;
		case 77:
			//ST2
			fpregs->st_space[8+byteoffset] = val;
			break;
		case 78:
			//ST3
			fpregs->st_space[12+byteoffset] = val;
			break;
		case 79:
			//ST4
			fpregs->st_space[16+byteoffset] = val;
			break;
		case 80:
			//ST5
			fpregs->st_space[20+byteoffset] = val;
			break;
		case 81:
			//ST6
			fpregs->st_space[24+byteoffset] = val;
			break;
		case 82:
			//ST7
			fpregs->st_space[28+byteoffset] = val;
			break;
		case 91:
			//XMM0
			fpregs->xmm_space[0+byteoffset] = val;
			break;
		case 92:
			//XMM1
			fpregs->xmm_space[4+byteoffset] = val;
			break;
		case 93:
			//XMM2
			fpregs->xmm_space[8+byteoffset] = val;
			break;
		case 94:
			//XMM3
			fpregs->xmm_space[12+byteoffset] = val;
			break;
		case 95:
			//XMM4
			fpregs->xmm_space[16+byteoffset] = val;
			break;
		case 96:
			//XMM5
			fpregs->xmm_space[20+byteoffset] = val;
			break;
		case 97:
			//XMM6
			fpregs->xmm_space[24+byteoffset] = val;
			break;
		case 98:
			//XMM7
			fpregs->xmm_space[28+byteoffset] = val;
			break;
		case 99:
			//XMM8
			fpregs->xmm_space[32+byteoffset] = val;
			break;
		case 100:
			//XMM9
			fpregs->xmm_space[36+byteoffset] = val;
			break;
		case 101:
			//XMM10
			fpregs->xmm_space[40+byteoffset] = val;
			break;
		case 102:
			//XMM11
			fpregs->xmm_space[44+byteoffset] = val;
			break;
		case 103:
			//XMM12
			fpregs->xmm_space[48+byteoffset] = val;
			break;
		case 104:
			//XMM13
			fpregs->xmm_space[52+byteoffset] = val;
			break;
		case 105:
			//XMM14
			fpregs->xmm_space[56+byteoffset] = val;
			break;
		case 106:
			//XMM15
			fpregs->xmm_space[60+byteoffset] = val;
			break;
		default:
			fprintf(stderr, "set_fpreg, unsupported register index\n");
			exit(EXIT_FAILURE);
	}
}

unsigned get_bitwidth(uint8_t index)
{
	switch(index)
	{
		case 0:
			//RAX
			return 64;
		case 1:
			//RCX
			return 64;
		case 2:
			//RDX
			return 64;
		case 3:
			//RBX
			return 64;
		case 4:
			//RSP
			return 64;
		case 5:
			//RBP
			return 64;
		case 6:
			//RSI
			return 64;
		case 7:
			//RDI
			return 64;
		case 8:
			//R8
			return 64;
		case 9:
			//R9
			return 64;
		case 10:
			//R10
			return 64;
		case 11:
			//R11
			return 64;
		case 12:
			//R12
			return 64;
		case 13:
			//R13
			return 64;
		case 14:
			//R14
			return 64;
		case 15:
			//R15
			return 64;
		case 16:
			//EAX
			return 32;
		case 17:
			//ECX
			return 32;
		case 18:
			//EDX
			return 32;
		case 19:
			//EBX
			return 32;
		case 20:
			//ESP
			return 32;
		case 21:
			//EBP
			return 32;
		case 22:
			//ESI
			return 32;
		case 23:
			//EDI
			return 32;
		case 24:
			//R8D
			return 32;
		case 25:
			//R9D
			return 32;
		case 26:
			//R10D
			return 32;
		case 27:
			//R11D
			return 32;
		case 28:
			//R12D
			return 32;
		case 29:
			//R13D
			return 32;
		case 30:
			//R14D
			return 32;
		case 31:
			//R15D
			return 32;
		case 32:
			//AX
			return 16;
		case 33:
			//CX
			return 16;
		case 34:
			//DX
			return 16;
		case 35:
			//BX
			return 16;
		case 36:
			//SP
			return 16;
		case 37:
			//BP
			return 16;
		case 38:
			//SI
			return 16;
		case 39:
			//DI
			return 16;
		case 40:
			//R8W
			return 16;
		case 41:
			//R9W
			return 16;
		case 42:
			//R10W
			return 16;
		case 43:
			//R11W
			return 16;
		case 44:
			//R12W
			return 16;
		case 45:
			//R13W
			return 16;
		case 46:
			//R14W
			return 16;
		case 47:
			//R15W
			return 16;
		case 48:
			//AL
			return 8;
		case 49:
			//CL
			return 8;
		case 50:
			//DL
			return 8;
		case 51:
			//BL
			return 8;
		case 52:
			//AH
			return 8;
		case 53:
			//CH
			return 8;
		case 54:
			//DH
			return 8;
		case 55:
			//BH
			return 8;
		case 56:
			//R8B
			return 8;
		case 57:
			//R9B
			return 8;
		case 58:
			//R10B
			return 8;
		case 59:
			//R11B
			return 8;
		case 60:
			//R12B
			return 8;
		case 61:
			//R13B
			return 8;
		case 62:
			//R14B
			return 8;
		case 63:
			//R15B
			return 8;
		case 64:
			//SPL
			return 8;
		case 65:
			//BPL
			return 8;
		case 66:
			//SIL
			return 8;
		case 67:
			//DIL
			return 8;
		default:
			fprintf(stderr, "unsupported register index\n");
			exit(EXIT_FAILURE);
	}
}

unsigned is_high(uint8_t index)
{
	switch(index)
	{
		case 52:
			//AH
			return 1;
		case 53:
			//CH
			return 1;
		case 54:
			//DH
			return 1;
		case 55:
			//BH
			return 1;
		default:
			return 0;
	}
}

const char* get_regname(uint8_t index)
{
	switch(index)
	{
		case 0:
			return "RAX";
		case 1:
			return "RCX";
		case 2:
			return "RDX";
		case 3:
			return "RBX";
		case 4:
			return "RSP";
		case 5:
			return "RBP";
		case 6:
			return "RSI";
		case 7:
			return "RDI";
		case 8:
			return "R8";
		case 9:
			return "R9";
		case 10:
			return "R10";
		case 11:
			return "R11";
		case 12:
			return "R12";
		case 13:
			return "R13";
		case 14:
			return "R14";
		case 15:
			return "R15";
		case 16:
			return "EAX";
		case 17:
			return "ECX";
		case 18:
			return "EDX";
		case 19:
			return "EBX";
		case 20:
			return "ESP";
		case 21:
			return "EBP";
		case 22:
			return "ESI";
		case 23:
			return "EDI";
		case 24:
			return "R8D";
		case 25:
			return "R9D";
		case 26:
			return "R10D";
		case 27:
			return "R11D";
		case 28:
			return "R12D";
		case 29:
			return "R13D";
		case 30:
			return "R14D";
		case 31:
			return "R15D";
		case 32:
			return "AX";
		case 33:
			return "CX";
		case 34:
			return "DX";
		case 35:
			return "BX";
		case 36:
			return "SP";
		case 37:
			return "BP";
		case 38:
			return "SI";
		case 39:
			return "DI";
		case 40:
			return "R8W";
		case 41:
			return "R9W";
		case 42:
			return "R10W";
		case 43:
			return "R11W";
		case 44:
			return "R12W";
		case 45:
			return "R13W";
		case 46:
			return "R14W";
		case 47:
			return "R15W";
		case 48:
			return "AL";
		case 49:
			return "CL";
		case 50:
			return "DL";
		case 51:
			return "BL";
		case 52:
			return "AH";
		case 53:
			return "CH";
		case 54:
			return "DH";
		case 55:
			return "BH";
		case 56:
			return "R8B";
		case 57:
			return "R9B";
		case 58:
			return "R10B";
		case 59:
			return "R11B";
		case 60:
			return "R12B";
		case 61:
			return "R13B";
		case 62:
			return "R14B";
		case 63:
			return "R15B";
		case 64:
			return "SPL";
		case 65:
			return "BPL";
		case 66:
			return "SIL";
		case 67:
			return "DIL";
		case 68:
			return "ES";
		case 69:
			return "CS";
		case 70:
			return "SS";
		case 71:
			return "DS";
		case 72:
			return "FS";
		case 73:
			return "GS";
		case 74:
			return "RIP";
		case 75:
			return "ST0";
		case 76:
			return "ST1";
		case 77:
			return "ST2";
		case 78:
			return "ST3";
		case 79:
			return "ST4";
		case 80:
			return "ST5";
		case 81:
			return "ST6";
		case 82:
			return "ST7";
		case 83:
			return "MM0";
		case 84:
			return "MM1";
		case 85:
			return "MM2";
		case 86:
			return "MM3";
		case 87:
			return "MM4";
		case 88:
			return "MM5";
		case 89:
			return "MM6";
		case 90:
			return "MM7";
		case 91:
			return "XMM0";
		case 92:
			return "XMM1";
		case 93:
			return "XMM2";
		case 94:
			return "XMM3";
		case 95:
			return "XMM4";
		case 96:
			return "XMM5";
		case 97:
			return "XMM6";
		case 98:
			return "XMM7";
		case 99:
			return "XMM8";
		case 100:
			return "XMM9";
		case 101:
			return "XMM10";
		case 102:
			return "XMM11";
		case 103:
			return "XMM12";
		case 104:
			return "XMM13";
		case 105:
			return "XMM14";
		case 106:
			return "XMM15";
		case 107:
			return "YMM0";
		case 108:
			return "YMM1";
		case 109:
			return "YMM2";
		case 110:
			return "YMM3";
		case 111:
			return "YMM4";
		case 112:
			return "YMM5";
		case 113:
			return "YMM6";
		case 114:
			return "YMM7";
		case 115:
			return "YMM8";
		case 116:
			return "YMM9";
		case 117:
			return "YMM10";
		case 118:
			return "YMM11";
		case 119:
			return "YMM12";
		case 120:
			return "YMM13";
		case 121:
			return "YMM14";
		case 122:
			return "YMM15";
		case 123:
			return "CR0";
		case 125:
			return "CR2";
		case 126:
			return "CR3";
		case 127:
			return "CR4";
		case 131:
			return "CR8";
		case 132:
			return "DR0";
		case 133:
			return "DR1";
		case 134:
			return "DR2";
		case 135:
			return "DR3";
		case 138:
			return "DR6";
		case 139:
			return "DR7";
		default:
			return NULL;
	}
}

