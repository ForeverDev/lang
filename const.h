#ifndef __CONST_H
#define __CONST_H

#define S_STACK	2048
#define S_RAM	1024
#define S_FUNC	256
#define S_LABEL 256
#define S_REG   8
#define CODEMAX	16000

typedef unsigned char	u8;
typedef unsigned int	u32;

typedef signed char		s8;
typedef signed int		s32;

static const u32 opcodes[][2] = {
	{0x00, 0},		// EOF
	{0x01, 1},		// ICONST
	{0x02, 0},		// IADD
	{0x03, 0},		// ISUB
	{0x04, 0},		// IMUL
	{0x05, 1},		// DEFFC
	{0x06, 0},		// ENDFC
	{0x07, 2},		// CALL
	{0x08, 0},		// RET
	{0x09, 1},		// PRNT
	{0x0a, 1},	    // LABEL
	{0x0b, 1},	    // JMP
	{0x0c, 1},	    // JIT
	{0x0d, 1},	    // JIF
	{0x0e, 0},  	// LT
	{0x0f, 0},	    // LE
	{0x10, 0},  	// GT
	{0x11, 0},  	// GE
	{0x12, 0},  	// EQ
    {0x13, 0},  	// NEQ
    {0x14, 1},  	// LOAD
    {0x15, 0},      // SCONST (args set to 0 because sizeof str unknown)
    {0x16, 1},      // MOVI
    {0x17, 1},      // LOAD REG
    {0x18, 0},      // SLOAD
    {0x19, 0},      // PUSHSP

	// debugging commands
	{0xf0, 0},		// DS
	{0xf1, 0},		// DIP
	{0xf2, 0},		// DBP
	{0xf3, 0},		// DSP
	{0xf4, 0}		// DALL
};

#endif
