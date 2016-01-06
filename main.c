#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "const.h"

typedef struct _vm {
	s32 sp;	
	u32 ip;
	u32 bp;
	u32 funcs[S_FUNC];
	u32 labels[S_LABEL];
	s32 ram[S_RAM];
	s32 stack[S_STACK];
} VM;

VM* VM_new();
void VM_dumpStack(VM*);
void VM_run(VM*, const s32*);
void VM_runFromString(VM*, const char*);

VM* VM_new() {
	VM* vm = malloc(sizeof(VM));
	vm->sp = -1;
	vm->ip = 0;
	vm->bp = 0;
	memset(vm->funcs, 0, sizeof(vm->funcs));
	memset(vm->labels, 0, sizeof(vm->labels));
	return vm;
}

void VM_dumpStack(VM* vm) {
	for (int i = 0; i <= vm->sp; i++) {
		printf("%04d: %d\n", i, vm->stack[i]);
	}
}

void VM_run(VM* vm, const s32* code) {
	while (code[vm->ip] != 0x00) {
		u32 opcode = code[vm->ip];
		vm->ip++;
		switch (opcode) {
			case 0x05:
				vm->funcs[code[vm->ip++]] = vm->ip + 1;
				break;
			case 0x0a:
				vm->labels[code[vm->ip++]] = vm->ip + 1;
				break;
			default:
				vm->ip += opcodes[opcode][1];
				break;
		}
	}
	vm->ip = 0;
	while (code[vm->ip] != 0x00) {
		if (vm->sp >= S_STACK) {
			printf("stack overflow\n");
			return;
		}
		u32 opcode = code[vm->ip];
		vm->ip++;
		switch (opcode) {
			case 0x01:
				vm->stack[++vm->sp] = code[vm->ip++];
				break;
			case 0x02:
				vm->stack[++vm->sp] = vm->stack[vm->sp--] + vm->stack[vm->sp--];
				break;
			case 0x03:
				vm->stack[++vm->sp] = vm->stack[vm->sp--] - vm->stack[vm->sp--];
				break;
			case 0x04:
				vm->stack[++vm->sp] = vm->stack[vm->sp--] * vm->stack[vm->sp--];
				break;
			case 0x05:
				while (code[vm->ip] != 0x06) {
					vm->ip++;
				}
				vm->ip++;
				break;
			case 0x06:
				break;
			case 0x07: {
				int flocation = code[vm->ip++];
				vm->stack[++vm->sp] = code[vm->ip++];
				vm->stack[++vm->sp] = vm->bp;
				vm->stack[++vm->sp] = vm->ip;
				vm->bp = vm->sp;
				vm->ip = vm->funcs[flocation];
				break;
			}
			case 0x08: {
				int ret = vm->stack[vm->sp--];
				vm->sp = vm->bp;
				vm->ip = vm->stack[vm->sp--];
				vm->bp = vm->stack[vm->sp--];
				vm->sp -= vm->stack[vm->sp--];
				vm->stack[++vm->sp] = ret;
				break;
			}
			case 0x09:
				printf("PRNT: %d\n", vm->stack[vm->sp--]);
				break;
			case 0x0a:
				break;
			case 0x0b:
				vm->ip = vm->labels[code[vm->ip++]];
				break;
			case 0x0c:
				if (vm->stack[vm->sp--]) {
					vm->ip = vm->labels[code[vm->ip++]];
				} else {
					vm->ip++;
				}
				break;
			case 0x0d:
				if (!vm->stack[vm->sp--]) {
					vm->ip = vm->labels[code[vm->ip++]];		
				} else {
					vm->ip++;
				}
				break;
			// note that inequality comparisons return the opposite because of the
			// order of the stack
			case 0x0e:
				vm->stack[++vm->sp] = vm->stack[vm->sp--] < vm->stack[vm->sp--];
				break;
			case 0x0f:
				vm->stack[++vm->sp] = vm->stack[vm->sp--] <= vm->stack[vm->sp--];
				break;
			case 0x10:
				vm->stack[++vm->sp] = vm->stack[vm->sp--] > vm->stack[vm->sp--];
				break;
			case 0x11:
				vm->stack[++vm->sp] = vm->stack[vm->sp--] >= vm->stack[vm->sp--];
				break;
			case 0x12:
				vm->stack[++vm->sp] = vm->stack[vm->sp--] == vm->stack[vm->sp--];
				break;
			case 0x13:
				vm->stack[++vm->sp] = vm->stack[vm->sp--] != vm->stack[vm->sp--];
				break;
			case 0x14:
				vm->stack[++vm->sp] = vm->stack[vm->bp + code[vm->ip++]];
				break;
			case 0xf0:
				VM_dumpStack(vm);
				break;
			case 0xf1:
				printf("IP: %04d\n", vm->ip);
				break;
			case 0xf2:
				printf("BP: %04d\n", vm->bp);
				break;
			case 0xf3:
				printf("SP: %04d\n", vm->sp);
				break;
			case 0xf4:
				VM_dumpStack(vm);
				printf("IP: %04d\n", vm->ip);
				printf("BP: %04d\n", vm->bp);
				printf("SP: %04d\n", vm->sp);
				break;
		}	
	}
}

void VM_runFromString(VM* vm, const char* code) {
	s32 generated_code[1024];
	int index = 0, bufptr = 0, codeptr = 0;
	char buf[128];
	while (1) {
		if (code[index] == ' ' || code[index] == '\0') {
			char* hexa = malloc(bufptr);
			for (int i = 0; i < bufptr; i++) {
				hexa[i] = buf[i];
			}
			bufptr = 0;
			memset(buf, 0, sizeof(buf));
			generated_code[codeptr++] = (s32)strtol(hexa, NULL, 10);
			free(hexa);
			if (code[index] == '\0') {
				break;
			}
		} else {
			buf[bufptr++] = code[index];
		}
		index++;
	}
	VM_run(vm, generated_code);
}

int main(int argc, char** argv) {

	VM* vm = VM_new();	
	const char* code =  "05 00 01 01 20 -3 18 13 00 01 01 08 10 00 01 01 20 -3 03 07 00 01 20 -3 04 08 06 01 05 07 00 01 09 00";

	VM_runFromString(vm, code);

	return 0;

}
