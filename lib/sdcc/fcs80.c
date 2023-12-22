/**
 * @file fcs80.c
 * @brief FCS80 system call implementation for SDCC
 * -----------------------------------------------------------------------------
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Yoji Suzuki.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * -----------------------------------------------------------------------------
 */
#include "fcs80.h"
#pragma disable_warning 59 // no check none return (return at inline-asm)
#pragma disable_warning 85 // no check unused args (check at inline-asm)

void fcs80_wait_vsync(void)
{
__asm
    ld hl, #0x9607
wait_vblank_loop:
    ld a, (hl)
    and #0x80
    jp z, wait_vblank_loop
__endasm;
}

void fcs80_wait_scanline(uint8_t n)
{
    if (192 <= n) {
        return; // invalid line number
    }
    while (n != *((volatile uint8_t*)0x9600)) {
        ;
    }
}

void fcs80_palette_set(uint8_t pn, uint8_t pi, uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t col;
    uint16_t addr;
    col = r & 0x1F;
    col <<= 5;
    col |= g & 0x1F;
    col <<= 5;
    col |= b & 0x1F;
    addr = 0x9400;
    addr += pn << 5;
    addr += pi << 1;
    *((uint16_t*)addr) = col;
}

void fcs80_dma(uint8_t prg)
{
__asm
    out (#0xC0), a
__endasm;
}

NameTable* fcs80_bg_get_nametbl(void)
{
    return (NameTable*)0x8000;
}

void fcs80_bg_putstr(uint8_t x, uint8_t y, uint8_t attr, const char* str)
{
    x &= 0x1F;
    y &= 0x1F;
    uint16_t addrC = 0x8000 + (y << 5) + x;
    uint16_t addrA = addrC + 0x400;
    while (*str) {
        *((uint8_t*)addrC) = *str;
        *((uint8_t*)addrA) = attr;
        addrC++;
        addrA++;
        str++;
    }
}

void fcs80_bg_scroll(uint8_t x, uint8_t y)
{
    *((uint8_t*)0x9602) = x;
    *((uint8_t*)0x9603) = y;
}

void fcs80_bg_scroll_x(uint8_t x)
{
    *((uint8_t*)0x9602) = x;
}

void fcs80_bg_scroll_y(uint8_t y)
{
    *((uint8_t*)0x9603) = y;
}

OAM* fcs80_get_oam(void)
{
    return (OAM*)0x9000;
}

uint8_t fcs80_joypad_get(void)
{
__asm
    ld a, #0x0E
    out (#0xA0), a
    in a, (#0xA2)
    xor #0xFF
    ld l, a
    ret
__endasm;
}
