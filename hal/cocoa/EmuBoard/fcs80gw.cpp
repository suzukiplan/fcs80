//
//  fcs80gw.cpp
//  EmuBoard
//
//  Created by Yoji Suzuki on 2021/03/06.
//  Copyright © 2021 SUZUKIPLAN. All rights reserved.
//

#include "fcs80gw.h"
#include "fcs80.hpp"

void* fcs80_create() { return new FCS80(); }
void fcs80_destroy(void* ctx) { delete (FCS80*)ctx; }
void fcs80_reset(void* ctx) { ((FCS80*)ctx)->reset(); }
void fcs80_loadRom(void* ctx, const void* rom, size_t size) { ((FCS80*)ctx)->loadRom(rom, size); }
void fcs80_tick(void* ctx, unsigned char pad1, unsigned char pad2) { ((FCS80*)ctx)->tick(pad1, pad2); }
unsigned short* fcs80_display(void* ctx) { return ((FCS80*)ctx)->getDisplay(); }
short* fcs80_sound(void* ctx, size_t* size) { return ((FCS80*)ctx)->dequeSoundBuffer(size); }
size_t fcs80_stateSize(void* ctx) { return ((FCS80*)ctx)->getStateSize(); }
void fcs80_stateSave(void* ctx, void* buffer) { ((FCS80*)ctx)->saveState(buffer); }
void fcs80_stateLoad(void* ctx, const void* buffer) { ((FCS80*)ctx)->loadState(buffer); }
unsigned char* fcs80_getVRAM(void* ctx) { return ((FCS80*)ctx)->vdp->ctx.ram; }
