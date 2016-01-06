#ifndef __CONST_H
#define __CONST_H

#define S_STACK	2048
#define S_RAM	1024
#define S_FUNC	256
#define S_LABEL 256
#define CODEMAX	16000

typedef unsigned char	u8;
typedef unsigned int	u32;

typedef signed char		s8;
typedef signed int		s32;

static const u32 opcodes[][2] = {
	{0, 0},		// EOF
	{1, 1},		// ICONST
	{2, 0},		// IADD
	{3, 0},		// ISUB
	{4, 0},		// IMUL
	{5, 1},		// DEFFC
	{6, 0},		// ENDFC
	{7, 2},		// CALL
	{8, 0},		// RET
	{9, 0},		// PRNT
	{10, 1},	// LABEL
	{11, 1},	// JMP
	{12, 1},	// JIT
	{13, 1},	// JIF
	{14, 0},	// LT
	{15, 0},	// LE
	{16, 0},	// GT
	{17, 0},	// GE
	{18, 0},	// EQ
	{19, 0},	// NEQ
	{20, 1},	// LOAD

	// debugging commands
	{0xf0, 0},		// DS
	{0xf1, 0},		// DIP
	{0xf2, 0},		// DBP
	{0xf3, 0},		// DSP
	{0xf4, 0}		// DALL
};

#endif
