//
//  main.c
//  emulator.c
//
//  Created on 05.10.15.
//  Copyright © 2015 com.mipt. All rights reserved.
// 8a
// 05
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "emulator.c"

int main(void) {
    emu_init();
    emu_step();
    
    return 0;
}