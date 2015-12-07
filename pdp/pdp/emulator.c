//
//  emulator.c
//  emulator.c
//
//  Created on 06.10.15.
//  Copyright © 2015 com.mipt. All rights reserved.
//

#include "emulator.h"
#include "instruction_table.c"

//| 005000
// | 0000101000000000
union Bytik {
    struct {
        unsigned b1: 1;
        unsigned b2: 1;
        unsigned b3: 1;
        unsigned b4: 1;
        unsigned b5: 1;
        unsigned b6: 1;
        unsigned b7: 1;
        unsigned b8: 1;
    };
    uint8_t c;
};
union Result {
    struct {
        unsigned b1: 1;
        unsigned b2: 1;
        unsigned b3: 1;
        unsigned b4: 1;
        unsigned b5: 1;
        unsigned b6: 1;
        unsigned b7: 1;
        unsigned b8: 1;
        unsigned b9: 1;
        unsigned b10: 1;
        unsigned b11: 1;
        unsigned b12: 1;
        unsigned b13: 1;
        unsigned b14: 1;
        unsigned b15: 1;
        unsigned b16: 1;
    };
    uint16_t res;
};

unsigned char Invert(unsigned char x)
{
    int base = 256;
    
    unsigned char  res = 0;
    while (x != 0)
    {
        res += (x & 1) * (base >>= 1);
        x >>= 1;
    }
    
    return res;
}

int init_memory() { // 00050
    int i = 0;
    int b1, b2;
    
    for (i = 0; i < MEMORY_SIZE; i++) {
        memory.memory[i] = 0;
    }
    for (i = RAM_SIZE + VRAM_SIZE; i < MEMORY_SIZE; i+=2) {
        if (fscanf(file, "%x\n%x\n", &b1, &b2) >= 0) {
            memory.memory[i++] = b2;
            memory.memory[i] = b1;
            //printf("%d %d %d %d %d %d %d %d\n", b.b1, b.b2, b.b3, b.b4, b.b5, b.b6, b.b7, b.b8);
            printf("%x %x\n", b1, b2);
        }
        else {
            break;
        }
       // printf("\n%d finished, new\n",i);
    }
    
    return 0;
}

int init_registers() {
    int i;
    
    for (i = 0; i < 6; i++) {
        memory.R[i] = i + i*16 + i*256 + i*4096 + 12;
    }
    memory.R[6] = RAM_SIZE + VRAM_SIZE;
    
    return 0;
}

int init_flags() {
    flags.C = 0;
    flags.N = 0;
    flags.V = 0;
    flags.Z = 0;
    
    return 0;
}

int emu_init() {
    file = fopen("source.txt", "r");
    init_memory();
    init_registers();
    fill_table();
    int i;
    
    stop = FALSE;
    fclose(file);
    return 0;
}

int emu_reset() {
    stop = TRUE;
    init_memory();
    init_registers();
    init_flags();
    
    
    return 0;
}

byte read_mem_byte(int addr) {
    memory.R[6]++;
    return memory.memory[addr];
}

word read_mem_word(int addr) {
   //union Result result;
    memory.R[6] += 2;
   // union Bytik one, two;
    
    //printf("do %hhx %hhx lol = %ho \n", memory.ROM[0], memory.ROM[1], *(word*)&memory.memory[addr]); //| 0000 0000 1010 0000
   // result.res = *(word *)&memory.memory[addr];
  /*  memset(&result.res, 0, 2);
    
      printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d \n", result.b1, result.b2, result.b3, result.b4, result.b5, result.b6, result.b7, result.b8, result.b9, result.b10, result.b11, result.b12, result.b13, result.b14, result.b15, result.b16);
    memcpy(&result.res, &memory.memory[addr + 1], 1);
      printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d \n", result.b1, result.b2, result.b3, result.b4, result.b5, result.b6, result.b7, result.b8, result.b9, result.b10, result.b11, result.b12, result.b13, result.b14, result.b15, result.b16);
    one.c = memory.memory[addr];
    two.c = memory.memory[addr + 1];
    printf("two %d %d %d %d %d %d %d %d\n", two.b1, two.b2, two.b3, two.b4, two.b5, two.b6, two.b7, two.b8);
    memcpy(&result.res + sizeof(byte), &memory.memory[addr], 1);
   printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d \n", result.b1, result.b2, result.b3, result.b4, result.b5, result.b6, result.b7, result.b8, result.b9, result.b10, result.b11, result.b12, result.b13, result.b14, result.b15, result.b16);
    printf("lll %d %d\n", one.c, two.c);*/
   // printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d \n", result.b1, result.b2, result.b3, result.b4, result.b5, result.b6, result.b7, result.b8, result.b9, result.b10, result.b11, result.b12, result.b13, result.b14, result.b15, result.b16);
    return *(word*)&memory.memory[addr];
}

int check_instr(int i, instruction in) {
    return (in.instr >= table[i].instruction_diapason.first.instr)&&(in.instr <= table[i].instruction_diapason.last.instr);
}
// 
int emu_step() {
    instruction in;
    int i;
    
    in.instr = read_mem_word(memory.R[6]);
   
    printf("lol\n");
    for (i = 0; i < COUNT; i++) { // 012700 0 001 010 1   1100   0000 000100 0 000 000 0  01 00  0 000
        if (check_instr(i, in)) {
            printf("%s  %x %x\n", table[i].assembler(in), memory.R[0], memory.R[5]);
            handle_callback(i, in);
            printf("%s  %x %x\n", table[i].assembler(in), memory.R[0], memory.R[5]);
            //printf("%s r0 = %d\n", table[i].assembler(in), registers.R[5]);
            printf(" N %d | Z %d | V %d | C %d\n", flags.N, flags.Z, flags.V, flags.C);
            break;
        }
    }
    
    return 0;
}