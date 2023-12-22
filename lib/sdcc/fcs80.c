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

void fcs80_memset(uint16_t dst, uint8_t value, uint16_t cnt)
{
__asm
    ld b, h
    ld c, l
    push ix
    ld ix, #4
    add ix, sp
    ld a, (ix)
    inc ix
    ld l, (ix)
    inc ix
    ld h, (ix)
    pop ix
    out (#0xC2), a
__endasm;
}

void fcs80_memcpy(uint16_t dst, uint16_t src, uint16_t cnt)
{
__asm
    ld b, h
    ld c, l
    push ix
    ld ix, #4
    add ix, sp
    ld l, (ix)
    inc ix
    ld h, (ix)
    pop ix
    out (#0xC3), a
__endasm;
}

NameTable* fcs80_bg_nametbl_get(void)
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

NameTable* fcs80_fg_nametbl_get(void)
{
    return (NameTable*)0x8800;
}

void fcs80_fg_putstr(uint8_t x, uint8_t y, uint8_t attr, const char* str)
{
    x &= 0x1F;
    y &= 0x1F;
    uint16_t addrC = 0x8800 + (y << 5) + x;
    uint16_t addrA = addrC + 0x400;
    while (*str) {
        *((uint8_t*)addrC) = *str;
        *((uint8_t*)addrA) = attr;
        addrC++;
        addrA++;
        str++;
    }
}

void fcs80_fg_scroll(uint8_t x, uint8_t y)
{
    *((uint8_t*)0x9604) = x;
    *((uint8_t*)0x9605) = y;
}

void fcs80_fg_scroll_x(uint8_t x)
{
    *((uint8_t*)0x9604) = x;
}

void fcs80_fg_scroll_y(uint8_t y)
{
    *((uint8_t*)0x9605) = y;
}

OAM* fcs80_oam_get(void)
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

void fcs80_psg_tone_ch0_set(uint16_t tone)
{
__asm
    ld a, l
    out (#0xD0), a
    ld a, h
    out (#0xD1), a
__endasm;
}

uint16_t fcs80_psg_tone_ch0_get(void)
{
__asm
    in a, (#0xD0)
    ld l, a
    in a, (#0xD1)
    ld h, a
    ret
__endasm;
}

void fcs80_psg_tone_ch1_set(uint16_t tone)
{
__asm
    ld a, l
    out (#0xD2), a
    ld a, h
    out (#0xD3), a
__endasm;
}

uint16_t fcs80_psg_tone_ch1_get(void)
{
__asm
    in a, (#0xD2)
    ld l, a
    in a, (#0xD3)
    ld h, a
    ret
__endasm;
}

void fcs80_psg_tone_ch2_set(uint16_t tone)
{
__asm
    ld a, l
    out (#0xD4), a
    ld a, h
    out (#0xD5), a
__endasm;
}

uint16_t fcs80_psg_tone_ch2_get(void)
{
__asm
    in a, (#0xD4)
    ld l, a
    in a, (#0xD5)
    ld h, a
    ret
__endasm;
}

void fcs80_psg_tone_set(uint8_t ch, uint16_t tone)
{
    if (0 == ch) {
        fcs80_psg_tone_ch0_set(tone);
    } else if (1 == ch) {
        fcs80_psg_tone_ch1_set(tone);
    } else if (2 == ch) {
        fcs80_psg_tone_ch2_set(tone);
    }
}

uint16_t fcs80_psg_tone_get(uint8_t ch)
{
    if (0 == ch) {
        return fcs80_psg_tone_ch0_get();
    } else if (1 == ch) {
        return fcs80_psg_tone_ch1_get();
    } else if (2 == ch) {
        return fcs80_psg_tone_ch2_get();
    }
    return 0xFFFF;
}

void fcs80_psg_noise_set(uint8_t noise)
{
__asm
    out (#0xD6), a
__endasm;
}

uint8_t fcs80_psg_noise_get(void)
{
__asm
    in a, (#0xD6)
    ld l, a
    ret
__endasm;
}

void fcs80_psg_mixing_set(uint8_t mixing)
{
__asm
    out (#0xD7), a
__endasm;
}

uint8_t fcs80_psg_mixing_get(void)
{
__asm
    in a, (#0xD7)
    ld l, a
    ret
__endasm;
}

void fcs80_psg_volume_ch0_set(uint8_t volume)
{
__asm
    out (#0xD8), a
__endasm;
}

uint8_t fcs80_psg_volume_ch0_get(void)
{    
__asm
    in a, (#0xD8)
    ld l, a
    ret
__endasm;
}

void fcs80_psg_volume_ch1_set(uint8_t volume)
{
__asm
    out (#0xD9), a
__endasm;
}

uint8_t fcs80_psg_volume_ch1_get(void)
{    
__asm
    in a, (#0xD9)
    ld l, a
    ret
__endasm;
}

void fcs80_psg_volume_ch2_set(uint8_t volume)
{
__asm
    out (#0xDA), a
__endasm;
}

uint8_t fcs80_psg_volume_ch2_get(void)
{    
__asm
    in a, (#0xDA)
    ld l, a
    ret
__endasm;
}

void fcs80_psg_volume_set(uint8_t ch, uint8_t volume)
{
    if (0 == ch) {
        fcs80_psg_volume_ch0_set(volume);
    } else if (1 == ch) {
        fcs80_psg_volume_ch1_set(volume);
    } else if (2 == ch) {
        fcs80_psg_volume_ch2_set(volume);
    }
}

uint8_t fcs80_psg_volume_get(uint8_t ch)
{
    if (0 == ch) {
        return fcs80_psg_volume_ch0_get();
    } else if (1 == ch) {
        return fcs80_psg_volume_ch1_get();
    } else if (2 == ch) {
        return fcs80_psg_volume_ch2_get();
    }
    return 0xFF;
}

void fcs80_psg_envelope_period_set(uint16_t period)
{
__asm
    ld a, l
    out (#0xDB), a
    ld a, h
    out (#0xDC), a
__endasm;
}

uint16_t fcs80_psg_envelope_period_get(void)
{
__asm
    in a, (#0xDB)
    ld l, a
    in a, (#0xDC)
    ld h, a
    ret
__endasm;
}

void fcs80_psg_envelope_pattern_set(uint8_t pattern)
{
__asm
    out (#0xDD), a
__endasm;
}

uint8_t fcs80_psg_envelope_pattern_get(void)
{
__asm
    in a, (#0xDD)
    ld l, a
    ret
__endasm;
}

void fcs80_scc_waveform_set(uint8_t ch, const void* waveform)
{
    if (4 <= ch) return;
    fcs80_memcpy(0x9800 + (ch << 5), (uint16_t)waveform, 32);
}

void fcs80_scc_waveform_get(uint8_t ch, void* waveform)
{
    if (4 <= ch) return;
    fcs80_memcpy((uint16_t)waveform, 0x9800 + (ch << 5), 32);
}
