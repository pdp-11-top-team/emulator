//
//  emulator.h
//  emulator.c
//
//  Created on 06.10.15.
//  Copyright © 2015 com.mipt. All rights reserved.
//

#ifndef emulator_h
#define emulator_h
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
//#include <sys/uio.h>
//#include <unistd.h>
#define REGISTERS 7
#define RAM_SIZE 8000
#define VRAM_SIZE 8000
#define ROM_SIZE 16000
#define MEMORY_SIZE (RAM_SIZE + VRAM_SIZE + ROM_SIZE + REGISTERS*2)
#define R0_INDEX (RAM_SIZE + VRAM_SIZE + ROM_SIZE)
#define FALSE 0
#define TRUE 1

typedef uint8_t byte;
typedef uint16_t word;

/*
struct Registers {
    word R[7];
} registers;*/

struct Flags {
    unsigned V: 1; // overflow
    unsigned C: 1; // carry
    unsigned Z: 1; // zero
    unsigned N: 1; // negativ
} flags;

union Memory {
    struct {
        byte RAM[RAM_SIZE];
        byte VRAM[VRAM_SIZE];
        byte ROM[ROM_SIZE];
        word R[REGISTERS];
    };
    byte memory[MEMORY_SIZE];
} memory;

int stop = FALSE;
FILE *file;

int emu_run();
int emu_reset();
int emu_step();
int init_memory();
unsigned char Invert(unsigned char x);

#endif /* emulator_h */
