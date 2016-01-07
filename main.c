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
	s32 stack[S_STACK];
    s32 reg[S_REG];
} VM;

VM* VM_new();
void VM_dumpStackAndReg(VM*);
void VM_run(VM*, const s32*);
void VM_runFromString(VM*, const char*);

VM* VM_new() {
	VM* vm = malloc(sizeof(VM));
	vm->sp = -1;
	vm->ip = 0;
	vm->bp = 0;
	memset(vm->funcs, 0, sizeof(vm->funcs));
	memset(vm->labels, 0, sizeof(vm->labels));
    memset(vm->reg, 0, sizeof(vm->reg));
	return vm;
}

void VM_dumpStackAndReg(VM* vm) {
    printf("STACK:\n");
	for (int i = 0; i <= vm->sp; i++) {
		printf("%04d: 0x%04x\n", i, vm->stack[i]);
	}
    printf("REGISTERS:\n");
    for (int i = 0; i < S_REG; i++) {
        printf("%04d: 0x%04x\n", i, vm->reg[i]);
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
            // if string const, skip over the string (it is null terminated)
            case 0x15:
                while (vm->labels[code[vm->ip]] != 0x00) {
                    vm->ip++;
                }
                vm->ip++;
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
                switch(code[vm->ip++]) {
                    // print int
                    case 0x00:
				        printf("%d", vm->stack[vm->sp--]);
                        break;
                    // print char
                    case 0x01:
                        printf("%c", vm->stack[vm->sp--]);
                        break;
                    // print newline
                    case 0x02:
                        printf("\n");
                        break;
                }
				break;
			case 0x0a:
                vm->ip++;
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
            // push the string backwards onto the stack so that the beginning of the string
            // is on the very top
            case 0x15: {
                int forward = 0;
                while (code[vm->ip + forward] != 0x00) {
                    forward++;
                }
                for (int i = forward; i >= 0; i--) {
                    vm->stack[++vm->sp] = code[vm->ip++];
                }
                break;
            }
            case 0x16:
                vm->reg[code[vm->ip++]] = vm->stack[vm->sp--];
                break;
            case 0x17:
                vm->stack[++vm->sp] = vm->reg[code[vm->ip++]];
                break;
            case 0x18:
                vm->stack[++vm->sp] = vm->stack[vm->stack[vm->sp--]];
                break;
            case 0x19:
                vm->stack[++vm->sp] = vm->sp;
                break;
			case 0xf0:
				VM_dumpStackAndReg(vm);
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
				VM_dumpStackAndReg(vm);
				printf("IP: 0x%04d\n", vm->ip);
				printf("BP: 0x%04d\n", vm->bp);
				printf("SP: 0x%04d\n", vm->sp);
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
			generated_code[codeptr++] = (s32)strtol(hexa, NULL, 16);
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
	const s32 code[] = {

        0x05, 0x00,
                0x14, -3,
                0x16, 0x00,
                0x0b, 0x01,
            0x0a, 0x00,
                0x17, 0x00,
                0x18,
                0x09, 0x01,
            0x0a, 0x01,
                0x17, 0x00,
                0x01, 0x01,
                0x02,
                0x16, 0x00,
                0x17, 0x00,
                0x18,
                0x01, 0x00,
                0x12,
                0x0d, 0x00,
                0x09, 0x02,

                0x08,
        0x06,

        0x15, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x00,
        0x01, 0x07,
        0x19,
        0x03,
        0x07, 0x00, 0x01,

        0x00

    };

	VM_run(vm, code);

	return 0;

}
