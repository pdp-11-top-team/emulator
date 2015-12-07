//
//  instruction_table.h
//  emulator.c
//
//  Created on 06.10.15.
//  Copyright © 2015 com.mipt. All rights reserved.
//

#ifndef instruction_table_h
#define instruction_table_h

#define COUNT 60
const int for_check = 1;
#define is_bigendian() ( (*(char*)&for_check) == 0 )
#define LEN 256
#define WRONG_DEST -1
#define MAXBYTE 127
#define MAXWORD 32767
#define NPLUSBYTE (*get_byte_from_memory(addr) & MAXBYTE)
#define NPLUSWORD (*get_word_from_memory(addr) & MAXWORD)
#define ZPLUSBYTE ((*get_byte_from_memory(addr) == 0) ? 1:0)
#define ZPLUSWORD ((*get_word_from_memory(addr) == 0) ? 1:0)

enum Type {
    SA = 1,
    DA,
    BR,
    CTR
};

typedef union Instruction {
    struct Da_instr {
        unsigned rd: 3; // destination register
        unsigned md: 3; // destination addressing method
        unsigned rs: 3; // sourse register
        unsigned ms: 3; // sourse addressing method
        unsigned opcode: 3;
        unsigned bw: 1; // byte - 1, world - 0
    } da_instr; // double address instruction
    struct Sa_instr {
        unsigned rd: 3;
        unsigned md: 3;
        unsigned opcode: 9;
        unsigned bw: 1; // 0000 0000 1010 0000
    } sa_instr; // single address instruction
    
    word instr;
} instruction;

typedef struct Field {
    struct Diapason {
        instruction first;
        instruction last;
    } instruction_diapason;
    int (*callback)(); //int addr, int addrs
    char *(*assembler)(instruction);
    int type;
} field;

field table[COUNT];
int table_index;

struct Operand {
        int r;
        int m;
        word *address;
};

byte *get_byte_from_memory(int address);
word *get_word_from_memory(int address);
/*
struct Destination get_rd(instruction instr);
byte *get_byte_from_memory(int address);
word *get_word_from_memory(int address);
int get_mdb(struct Destination *dest);
void set_flags(int n, int c, int z, int v);
int fill_table(void);
void clr(instruction instr);
char *clr_disas(instruction instr);*/

#endif /* instruction_table_h */
