//
//  fcs80gw.hpp
//  EmuBoard
//
//  Created by Yoji Suzuki on 2021/03/06.
//  Copyright © 2021 SUZUKIPLAN. All rights reserved.
//

#ifndef fcs80gw_hpp
#define fcs80gw_hpp

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void* fcs80_create(void);
void fcs80_destroy(void* ctx);
void fcs80_reset(void* ctx);
void fcs80_loadRom(void* ctx, const void* rom, size_t size);
void fcs80_tick(void* ctx, unsigned char pad1, unsigned char pad2);
unsigned short* fcs80_display(void* ctx);
short* fcs80_sound(void* ctx, size_t* size);
size_t fcs80_stateSize(void* ctx);
void fcs80_stateSave(void* ctx, void* buffer);
void fcs80_stateLoad(void* ctx, const void* buffer);
unsigned char* fcs80_getVRAM(void* ctx);

#ifdef __cplusplus
};
#endif

#endif /* fcs80gw_hpp */
