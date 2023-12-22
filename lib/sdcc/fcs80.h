/**
 * @file fcs80.h
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
#ifndef INCLUDE_FCS80_H
#define INCLUDE_FCS80_H

/** @def
 * Joypad bit mask (Cursor: Up)
 */
#define FCS80_JOYPAD_UP 0b10000000

/** @def
 * Joypad bit mask (Cursor: Down)
 */
#define FCS80_JOYPAD_DW 0b01000000

/** @def
 * Joypad bit mask (Cursor: Left)
 */
#define FCS80_JOYPAD_LE 0b00100000

/** @def
 * Joypad bit mask (Cursor: Right)
 */
#define FCS80_JOYPAD_RI 0b00010000

/** @def
 * Joypad bit mask (START button)
 */
#define FCS80_JOYPAD_ST 0b00001000

/** @def
 * Joypad bit mask (SELECT button)
 */
#define FCS80_JOYPAD_SE 0b00000100

/** @def
 * Joypad bit mask (A button: Jump/Bomber/Enter)
 */
#define FCS80_JOYPAD_T1 0b00000010

/** @def
 * Joypad bit mask (B button: Fire/Shot/Cancel)
 */
#define FCS80_JOYPAD_T2 0b00000001

//! stdint compatible (8bit unsigned)
typedef unsigned char uint8_t;

//! stdint compatible (8bit signed)
typedef signed char int8_t;

//! stdint compatible (16bit unsigned)
typedef unsigned short uint16_t;

//! stdint compatible (16bit signed)
typedef signed short int16_t;

//! stdint compatible (32bit unsigned)
typedef unsigned long uint32_t;

//! stdint compatible (32bit signed)
typedef signed long int32_t;

//! stdint compatible (64bit unsigned)
typedef unsigned long long uint64_t;

//! stdint compatible (64bit signed)
typedef signed long long int64_t;

/**
 * Object Attribute Memory record
 */
typedef struct {
    //! Y-coordinate
    uint8_t y;
    //! X-coordinate
    uint8_t x;
    //! Character pattern number
    uint8_t ptn;
    //! Attribute value https://github.com/suzukiplan/fcs80/blob/master/README.md#attribute-bit-layout-bgfgsprite
    uint8_t attr;
} OAM;

/**
 * Name Table
 */
typedef struct {
    //! Character pattern number
    uint8_t ptn[32][32];
    //! Attribute value https://github.com/suzukiplan/fcs80/blob/master/README.md#attribute-bit-layout-bgfgsprite
    uint8_t attr[32][32];
} NameTable;

/**
 * @brief Wait until V-Blank is detected
 */
void fcs80_wait_vsync(void);

/**
 * @brief Wait until specified scanline position
 * @param n scanline position (0-191)
 * @note A value of 192 or more for `n` causes an immediate return.
 */
void fcs80_wait_scanline(uint8_t n);

/**
 * @brief Set palette
 * @param pn number of palette (0-15)
 * @param pi index of palette (0-15)
 * @param r color depth of red (0-31)
 * @param g color depth of green (0-31)
 * @param b color depth of blue (0-31)
 */
void fcs80_palette_set(uint8_t pn, uint8_t pi, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief High-speed DMA transfer of the contents of the specified ROM Bank to the Character Pattern Table in VRAM
 * @param prg ROM Bank number (0-255)
 */
void fcs80_dma(uint8_t prg);

/**
 * @brief Acquire BG name table
 * @return BG name table
 */
NameTable* fcs80_bg_get_nametbl(void);

/**
 * @brief Continuously writes the specified ASCII code and attribute values to BG's NameTable
 * @param x X-coordinate (0-31)
 * @param y Y-coordinate (0-31)
 * @param attr attribute value https://github.com/suzukiplan/fcs80/blob/master/README.md#attribute-bit-layout-bgfgsprite
 * @param str '\0' terminated string
 */
void fcs80_bg_putstr(uint8_t x, uint8_t y, uint8_t attr, const char* str);

/**
 * @brief Set hardware scroll (X coordinate) for BG
 * @param x X-coordinate (0-255)
 */
void fcs80_bg_scroll_x(uint8_t x);

/**
 * @brief Set hardware scroll (X coordinate) for BG
 * @param y Y-coordinate (0-255)
 */
void fcs80_bg_scroll_y(uint8_t y);

/**
 * @brief Acquire the first address of OAM
 * @return first address of OAM
 * @note about OAM: https://github.com/suzukiplan/fcs80/blob/master/README.md#object-attribute-memory-sprite
 */
OAM* fcs80_get_oam(void);

/**
 * @brief Acquire joypad input status
 * @return joypad input status
 * @note that the input bit is originally reset (0) and the uninput bit is set (1), but when this function is used, it is inverted.
 */
uint8_t fcs80_joypad_get(void);

#endif
