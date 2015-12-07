//
//  instruction_table.c
//  emulator.c
//
//  Created on 06.10.15.
//  Copyright © 2015 com.mipt. All rights reserved.
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "instruction_table.h"
#include "emulator.h"

struct Operand get_rd(instruction instr) {
    struct Operand dest;
    
    if (instr.sa_instr.bw == 0) {
        dest.m = instr.sa_instr.md;
        dest.r = instr.sa_instr.rd;
        dest.address = get_word_from_memory(instr.sa_instr.rd*2 + R0_INDEX);
    }
    else {
        dest.m = instr.da_instr.md;
        dest.r = instr.da_instr.rd;
        dest.address = get_word_from_memory(instr.da_instr.rd*2 + R0_INDEX);
    }
    
    return dest;
}

struct Operand get_rs(instruction instr) {
    struct Operand source;
    
    source.m = instr.da_instr.ms;
    source.r = instr.da_instr.rs;
    source.address = get_word_from_memory(R0_INDEX + instr.da_instr.rd*2);
    
    return source;
}

byte *get_byte_from_memory(int address) {
    return &memory.memory[address];
}

word *get_word_from_memory(int address) {
    return (word *)&memory.memory[address];
}

void put_value_b(int address, byte value) {
    byte *dest_address;
    
    dest_address = get_byte_from_memory(address);
    *dest_address = value;
}

void put_value_w(int address, word value) {
    word *dest_address;
    
    dest_address = get_word_from_memory(address);
    *dest_address = value;
}

int get_opb(struct Operand *op) {
    int addr;
    
    switch (op->m) {
        case 0:
            return R0_INDEX + op->r*2;
        case 1:
            addr = *op->address;
            return addr;
        case 2:
            if (op->r == 7) {
                addr = *get_byte_from_memory(memory.R[6]);
            }
            else {
                addr = *op->address;
            }
            return addr;
        case 3:
            addr = *get_byte_from_memory(*op->address);
            return addr;
        case 4:
            addr = --(*op->address);
            return addr;
        case 5:
            addr = *get_byte_from_memory(--(*op->address));
            return addr;
        case 6:
            addr = (*op->address)+(*get_byte_from_memory(memory.R[6]));
            return addr;
        case 7:
            addr = *get_byte_from_memory((*op->address)+(*get_byte_from_memory(memory.R[6])));
            return addr;
    }
    
    return WRONG_DEST;
} // 0001dd
    // 000167 001 110111

int get_opw(struct Operand *op) {
    int addr;
    
    switch (op->m) {
        case 0:
            return R0_INDEX + op->r*2;
        case 1:
            addr = *op->address;
            return addr;
        case 2:
            if (op->r == 7) {
                addr = *get_byte_from_memory(memory.R[6]);
            }
            else {
                addr = *op->address;
            }
            //(*op->address) += 2;
            return addr;
        case 3:
            addr = *get_word_from_memory(*op->address);
            //(*op->address) += 2;
            return addr;
        case 4:
            *op->address -= 2;
            addr = *op->address;
            return addr;
        case 5:
            *op->address -= 2;
            addr = *get_word_from_memory(*op->address);
            return addr;
        case 6:
            addr = (*op->address)+(*get_word_from_memory(memory.R[6]));
            return addr;
        case 7:
            addr = *get_word_from_memory((*op->address)+(*get_word_from_memory(memory.R[6])));
            return addr;
    }
    
    return WRONG_DEST;
}


void handle_callback(int i, instruction instr) {
    int addr, addrs;
    int val = 0;
    struct Operand dest = get_rd(instr);
    struct Operand source;
    addr = get_opw(&dest);
    short disp;
    
    switch(table[i].type) {
   // if (table[i].type == SA) {
        case SA:
            table[i].callback(addr);
            if (instr.sa_instr.bw == 0) {
                val = 1;
            }
            else {
                val = 2;
            }
            break;
        case DA:
            source = get_rs(instr);
            addrs = get_opw(&source);
            table[i].callback(addr, addrs);
            if (instr.da_instr.bw == 0) {
                val = 1;
            }
            else {
                val = 2;
            }
            switch (source.m) {
                case 2: case 3:
                    source.address += val;
                case 6: case 7:
                    memory.R[6] += 2;
            }
            break;
        case BR:
            disp = MAXBYTE & instr.instr;
            if (table[i].callback() != 0) {
                memory.R[6] += 2*(short)disp;
            }
            return;
        case CTR:
            table[i].callback(addr);
            return;
        default:
            return;
    }
    
    switch (dest.m) {
        case 2: case 3:
            dest.address += val;
        case 6: case 7:
            memory.R[6] += 2;
    }
}


char *get_op_disas(struct Operand *op) {
    char *disas = (char *)malloc(LEN*sizeof(char));
    
    switch (op->m) {
        case 0:
            sprintf(disas, "%%R%d", op->r);
            return disas;
        case 2:
            if(op->r == 7) {
                sprintf(disas, "#%d", (*get_byte_from_memory(memory.R[6])));
                return disas;
            }
        case 1: case 4:
            sprintf(disas, "ox%x", *op->address);
            return disas;
        case 3: case 5:
            sprintf(disas, "ox%x", *get_byte_from_memory(*op->address));
            return disas;
        case 6:
            sprintf(disas, "ox%x", (*op->address)+(*get_byte_from_memory(memory.R[6])));
            return disas;
        default:
            sprintf(disas, "ox%x", *get_byte_from_memory((*op->address)+(*get_byte_from_memory(memory.R[6]))));
            return disas;
    }
}


void set_flags(int n, int z, int v, int c) {
    flags.N = n;
    flags.C = c;
    flags.Z = z;
    flags.V = v;
}

char *sa_instr_disas(instruction instr, char *op_name) {
    struct Operand dest = get_rd(instr);
    
    char *disas;
    char *dest_disas;
    
    disas = (char *)malloc(LEN*sizeof(char));
    dest_disas = get_op_disas(&dest);
    
    sprintf(disas, "%s %s", op_name, dest_disas);
    
    free(dest_disas);
    
    return disas;
}

char *da_instr_disas(instruction instr, char *op_name) {
    struct Operand dest = get_rd(instr);
    struct Operand source = get_rs(instr);
    
    char *disas;
    char *dest_disas, *source_disas;
    
    disas = (char *)malloc(LEN*sizeof(char));
    dest_disas = get_op_disas(&dest);
    source_disas = get_op_disas(&source);
 
    sprintf(disas, "%s %s, %s", op_name, source_disas, dest_disas);
    
    free(dest_disas);
    free(source_disas);
    
    return disas;
}

char *br_instr_disas(instruction instr, char *op_name) {
    char *disas;
    
    disas = (char *)malloc(LEN*sizeof(char));
    
    sprintf(disas, "%s ox%hx", op_name, (short)(instr.instr & MAXBYTE));
    
    return disas;
}

void clr(int addr) {
    put_value_w(addr, 0);
    set_flags(0, 1, 0, 0);
    
    return;
}

void clrb(int addr) {
    put_value_b(addr, 0);
    set_flags(0, 1, 0, 0);
}


char *clr_disas(instruction instr) {
    return sa_instr_disas(instr, "CLR");
}

char *clrb_disas(instruction instr) {
    return sa_instr_disas(instr, "CLRB");
}

void mov(int addr, int addrs) {
    put_value_w(addr, *get_word_from_memory(addrs));
    set_flags(NPLUSWORD, ZPLUSWORD, 0, flags.C);
}

void movb(int addr, int addrs) {
    put_value_b(addr, *get_byte_from_memory(addrs));
    set_flags(NPLUSBYTE, ZPLUSBYTE, 0, flags.C);
}


char *mov_disas(instruction instr) {
    return da_instr_disas(instr, "MOV");
}

char *movb_disas(instruction instr) {
    return da_instr_disas(instr, "MOVB");
}

void inc(int addr) {
    int v = 0;
    
    if (*get_word_from_memory(addr) == MAXWORD) {
        v = 1;
    }
    put_value_w(addr, *get_word_from_memory(addr) + 1);
    set_flags(NPLUSWORD, ZPLUSWORD, v, flags.C);
}

void incb(int addr) {
    int v = 0;
    
    if (*get_byte_from_memory(addr) == MAXBYTE) {
        v = 1;
    }
    put_value_b(addr, *get_byte_from_memory(addr) + 1);
    set_flags(NPLUSBYTE, ZPLUSBYTE, v, flags.C);
}

char *inc_disas(instruction instr) {
    return sa_instr_disas(instr, "INC");
}

char *incb_disas(instruction instr) {
    return sa_instr_disas(instr, "INCB");
}

void cmp(int addr) {
    put_value_w(addr, *get_word_from_memory(addr) + 1);
    set_flags(NPLUSWORD, ZPLUSWORD, 0, 0);
}

void cmpb(int addr) {
    put_value_b(addr, *get_byte_from_memory(addr) + 1);
    set_flags(NPLUSBYTE, ZPLUSBYTE, 0, 0);
}

char *cmp_disas(instruction instr) {
    return sa_instr_disas(instr, "CMP");
}

char *cmpb_disas(instruction instr) {
    return sa_instr_disas(instr, "CMPB");
}

int bne() {
    return flags.Z == 0;
}

char *bne_disas(instruction instr) {
    return br_instr_disas(instr, "BNE");
}

void jmp(int addr) {
    memory.R[6] = addr;
}

char *jmp_disas(instruction instr) {
    return sa_instr_disas(instr, "JMP");
}

void table_init() {
    table_index = 0;
}

void add(word first, word last, void *callback, void *assembler, int type) {
    table[table_index].instruction_diapason.first.instr = first;
    table[table_index].instruction_diapason.last.instr = last;
    table[table_index].callback = callback;
    table[table_index].assembler = assembler;
    table[table_index].type = type;
    
    table_index++;
}

int fill_table(void) {
    if(is_bigendian()) {
        return 0;
    }
    
    add(0105000, 0105077, clr, clr_disas, SA); 
    add(005000, 005077, clrb, clrb_disas, SA);
    add(0110000, 0117777, mov, mov_disas, DA);
    add(010000, 017777, movb, movb_disas, DA);
    add(0105200, 0105277, inc, inc_disas, SA);
    add(005200, 005277, incb, incb_disas, SA);
    add(0120000, 0127777, cmp, cmp_disas, DA);
    add(020000, 027777, cmpb, cmpb_disas, DA);
    add(001000, 001377, bne, bne_disas, BR);
    add(000100, 000177, jmp, jmp_disas, CTR);
    
    return 0;
}