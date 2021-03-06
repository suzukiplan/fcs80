//
//  EmuBoard.h
//  EmuBoard
//
//  Created by 鈴木　洋司　 on 2018/12/30.
//  Copyright © 2018年 SUZUKIPLAN. All rights reserved.
//

#include <stdio.h>
#include "constants.h"
extern unsigned short emu_vram[VRAM_WIDTH * VRAM_HEIGHT];
void emu_init(const void* rom, size_t romSize);
void emu_reload(const void* rom, size_t romSize);
void emu_reset(void);
void emu_vsync(void);
void emu_destroy(void);
const void* emu_saveState(size_t* size);
void emu_loadState(const void* state, size_t size);
void emu_dump(void);
