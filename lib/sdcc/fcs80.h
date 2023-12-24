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

/** @def
 * BG name table
 */
#define FCS80_ADDR_BG ((volatile NameTable*)0x8000)

/** @def
 * FG name table
 */
#define FCS80_ADDR_FG ((volatile NameTable*)0x8800)

/** @def
 * Object Attribute Memory
 */
#define FCS80_ADDR_OAM ((volatile OAM*)0x9000)

/** @def
 * Palette table
 */
#define FCS80_ADDR_PALETTE ((volatile uint16_t*)0x9400)

/** @def
 * Vertical rendering counter
 */
#define FCS80_ADDR_COUNT_V ((volatile uint8_t*)0x9600)

/** @def
 * Horizontal rendering counter
 */
#define FCS80_ADDR_COUNT_H ((volatile uint8_t*)0x9601)

/** @def
 * BG scroll X
 */
#define FCS80_ADDR_BG_SCROLL_X ((volatile uint8_t*)0x9602)

/** @def
 * BG scroll Y
 */
#define FCS80_ADDR_BG_SCROLL_Y ((volatile uint8_t*)0x9603)

/** @def
 * FG scroll X
 */
#define FCS80_ADDR_FG_SCROLL_X ((volatile uint8_t*)0x9604)

/** @def
 * FG scroll Y
 */
#define FCS80_ADDR_FG_SCROLL_Y ((volatile uint8_t*)0x9605)

/** @def
 * IRQ scanline position
 */
#define FCS80_ADDR_IRQ_SCANLINE ((volatile uint8_t*)0x9606)

/** @def
 * VDP status
 */
#define FCS80_ADDR_STATUS ((volatile uint8_t*)0x9607)

/** @def
 * Character pattern table
 */
#define FCS80_ADDR_CHARACTER ((volatile uint8_t*)0xA000)

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
void fcs80_wait_vsync(void) __z88dk_fastcall;

/**
 * @brief Wait until specified scanline position
 * @param n scanline position (0-191)
 * @note A value of 192 or more for `n` causes an immediate return.
 */
void fcs80_wait_scanline(uint8_t n) __z88dk_fastcall;

/**
 * @brief Set palette
 * @param pn number of palette (0-15)
 * @param pi index of palette (0-15)
 * @param r color depth of red (0-31)
 * @param g color depth of green (0-31)
 * @param b color depth of blue (0-31)
 */
void fcs80_palette_set(uint8_t pn, uint8_t pi, uint8_t r, uint8_t g, uint8_t b) __smallc;

/**
 * @brief Set palette (RGB555)
 * @param pn number of palette (0-15)
 * @param pi index of palette (0-15)
 * @param rgb555 color depth of RGB555 (0b0rrrrrgggggbbbbb)
 */
void fcs80_palette_set_rgb555(uint8_t pn, uint8_t pi, uint16_t rgb555) __smallc;

/**
 * @brief High-speed DMA transfer of the contents of the specified ROM Bank to the Character Pattern Table in VRAM
 * @param prg ROM Bank number (0-255)
 */
void fcs80_dma(uint8_t prg) __z88dk_fastcall;

/**
 * @brief DMA equivalent to `memset` in C language
 * @param dst destination address
 * @param value set value
 * @param cnt transfer count in byte
 */
void fcs80_memset(uint16_t dst, uint8_t value, uint16_t cnt) __smallc;

/**
 * @brief DMA equivalent to `memcpy` in C language
 * @param dst destination address
 * @param src source address
 * @param cnt transfer count in byte
 */
void fcs80_memcpy(uint16_t dst, uint16_t src, uint16_t cnt) __smallc;

/**
 * @brief Acquire BG name table
 * @return BG name table
 */
NameTable* fcs80_bg_nametbl_get(void) __smallc;

/**
 * @brief Continuously writes the specified ASCII code and attribute values to BG's NameTable
 * @param x X-coordinate (0-31)
 * @param y Y-coordinate (0-31)
 * @param attr attribute value https://github.com/suzukiplan/fcs80/blob/master/README.md#attribute-bit-layout-bgfgsprite
 * @param str '\0' terminated string
 */
void fcs80_bg_putstr(uint8_t x, uint8_t y, uint8_t attr, const char* str) __smallc;

/**
 * @brief Set hardware scroll (X and Y coordinate) for BG
 * @param x X-coordinate (0-255)
 * @param y Y-coordinate (0-255)
 */
void fcs80_bg_scroll(uint8_t x, uint8_t y) __smallc;

/**
 * @brief Set hardware scroll (X coordinate) for BG
 * @param x X-coordinate (0-255)
 */
void fcs80_bg_scroll_x(uint8_t x) __z88dk_fastcall;

/**
 * @brief Set hardware scroll (X coordinate) for BG
 * @param y Y-coordinate (0-255)
 */
void fcs80_bg_scroll_y(uint8_t y) __z88dk_fastcall;

/**
 * @brief Get hardware scroll (X coordinate) for BG
 * @return x X-coordinate (0-255)
 */
uint8_t fcs80_bg_scroll_x_get(void) __z88dk_fastcall;

/**
 * @brief Get hardware scroll (Y coordinate) for BG
 * @return y Y-coordinate (0-255)
 */
uint8_t fcs80_bg_scroll_y_get(void) __z88dk_fastcall;

/**
 * @brief Acquire FG name table
 * @return FG name table
 */
NameTable* fcs80_fg_nametbl_get(void) __smallc;

/**
 * @brief Continuously writes the specified ASCII code and attribute values to FG's NameTable
 * @param x X-coordinate (0-31)
 * @param y Y-coordinate (0-31)
 * @param attr attribute value https://github.com/suzukiplan/fcs80/blob/master/README.md#attribute-bit-layout-bgfgsprite
 * @param str '\0' terminated string
 */
void fcs80_fg_putstr(uint8_t x, uint8_t y, uint8_t attr, const char* str) __smallc;

/**
 * @brief Set hardware scroll (X and Y coordinate) for FG
 * @param x X-coordinate (0-255)
 * @param y Y-coordinate (0-255)
 */
void fcs80_fg_scroll(uint8_t x, uint8_t y) __smallc;

/**
 * @brief Set hardware scroll (X coordinate) for FG
 * @param x X-coordinate (0-255)
 */
void fcs80_fg_scroll_x(uint8_t x) __z88dk_fastcall;

/**
 * @brief Set hardware scroll (X coordinate) for FG
 * @param y Y-coordinate (0-255)
 */
void fcs80_fg_scroll_y(uint8_t y) __z88dk_fastcall;

/**
 * @brief Get hardware scroll (X coordinate) for FG
 * @return x X-coordinate (0-255)
 */
uint8_t fcs80_fg_scroll_x_get(void) __z88dk_fastcall;

/**
 * @brief Get hardware scroll (Y coordinate) for FG
 * @return y Y-coordinate (0-255)
 */
uint8_t fcs80_fg_scroll_y_get(void) __z88dk_fastcall;

/**
 * @brief Acquire the first address of OAM
 * @return first address of OAM
 * @note about OAM: https://github.com/suzukiplan/fcs80/blob/master/README.md#object-attribute-memory-sprite
 */
OAM* fcs80_oam_get(void) __smallc;

/**
 * @brief Acquire joypad input status
 * @return joypad input status
 * @note that the input bit is originally reset (0) and the uninput bit is set (1), but when this function is used, it is inverted.
 */
uint8_t fcs80_joypad_get(void) __z88dk_fastcall;

/**
 * @brief PSG: Set Ch0 tone
 * @param tone tone
 * @note about tone: https://github.com/suzukiplan/fcs80/blob/master/README.md#register-05-tone-generator
 */
void fcs80_psg_tone_ch0_set(uint16_t tone) __z88dk_fastcall;

/**
 * @brief PSG: Get Ch0 tone
 * @return tone
 * @note about tone: https://github.com/suzukiplan/fcs80/blob/master/README.md#register-05-tone-generator
 */
uint16_t fcs80_psg_tone_ch0_get(void) __z88dk_fastcall;

/**
 * @brief PSG: Set Ch1 tone
 * @param tone tone
 * @note about tone: https://github.com/suzukiplan/fcs80/blob/master/README.md#register-05-tone-generator
 */
void fcs80_psg_tone_ch1_set(uint16_t tone) __z88dk_fastcall;

/**
 * @brief PSG: Get Ch1 tone
 * @return tone
 */
uint16_t fcs80_psg_tone_ch1_get(void) __z88dk_fastcall;

/**
 * @brief PSG: Set Ch2 tone
 * @param tone tone
 * @note about tone: https://github.com/suzukiplan/fcs80/blob/master/README.md#register-05-tone-generator
 */
void fcs80_psg_tone_ch2_set(uint16_t tone) __z88dk_fastcall;

/**
 * @brief PSG: Get Ch2 tone
 * @return tone
 * @note about tone: https://github.com/suzukiplan/fcs80/blob/master/README.md#register-05-tone-generator
 */
uint16_t fcs80_psg_tone_ch2_get(void) __z88dk_fastcall;

/**
 * @brief PSG: Set tone
 * @param ch channel (0-2)
 * @param tone tone
 * @note about tone: https://github.com/suzukiplan/fcs80/blob/master/README.md#register-05-tone-generator
 */
void fcs80_psg_tone_set(uint8_t ch, uint16_t tone) __smallc;

/**
 * @brief PSG: Get tone
 * @param ch channel (0-2)
 * @return tone
 * @note about tone: https://github.com/suzukiplan/fcs80/blob/master/README.md#register-05-tone-generator
 */
uint16_t fcs80_psg_tone_get(uint8_t ch) __z88dk_fastcall;

/**
 * @brief PSG: Set noise
 * @param noise noise
 */
void fcs80_psg_noise_set(uint8_t noise) __z88dk_fastcall;

/**
 * @brief PSG: Get noise
 * @return noise
 */
uint8_t fcs80_psg_noise_get(void) __z88dk_fastcall;

/**
 * @brief PSG: Set mixing
 * @param mixing mixing
 */
void fcs80_psg_mixing_set(uint8_t mixing) __z88dk_fastcall;

/**
 * @brief PSG: Get mixing
 * @return mixing
 */
uint8_t fcs80_psg_mixing_get(void) __z88dk_fastcall;

/**
 * @brief PSG: Set Ch0 volume
 * @param volume volume
 */
void fcs80_psg_volume_ch0_set(uint8_t volume) __z88dk_fastcall;

/**
 * @brief PSG: Get Ch0 volume
 * @return volume
 */
uint8_t fcs80_psg_volume_ch0_get(void) __z88dk_fastcall;

/**
 * @brief PSG: Set Ch0 volume
 * @param volume volume
 */
void fcs80_psg_volume_ch1_set(uint8_t volume) __z88dk_fastcall;

/**
 * @brief PSG: Get Ch1 volume
 * @return volume
 */
uint8_t fcs80_psg_volume_ch1_get(void) __z88dk_fastcall;

/**
 * @brief PSG: Set Ch2 volume
 * @param volume volume
 */
void fcs80_psg_volume_ch2_set(uint8_t volume) __z88dk_fastcall;

/**
 * @brief PSG: Get Ch2 volume
 * @return volume
 */
uint8_t fcs80_psg_volume_ch2_get(void) __z88dk_fastcall;

/**
 * @brief PSG: Set volume
 * @param ch channel
 * @param volume volume
 */
void fcs80_psg_volume_set(uint8_t ch, uint8_t volume) __smallc;

/**
 * @brief PSG: Get volume
 * @param ch channel
 * @return volume
 */
uint8_t fcs80_psg_volume_get(uint8_t ch) __z88dk_fastcall;

/**
 * @brief PSG: Set envelope period
 * @param period envelope period
 */
void fcs80_psg_envelope_period_set(uint16_t period) __z88dk_fastcall;

/**
 * @brief PSG: Get envelope period
 * @return envelope period
 */
uint16_t fcs80_psg_envelope_period_get(void) __z88dk_fastcall;

/**
 * @brief PSG: Set envelope pattern
 * @param pattern envelope pattern
 */
void fcs80_psg_envelope_pattern_set(uint8_t pattern) __z88dk_fastcall;

/**
 * @brief PSG: Get envelope pattern
 * @return envelope pattern
 */
uint8_t fcs80_psg_envelope_pattern_get(void) __z88dk_fastcall;

/**
 * @brief SCC: Set Ch0 waveform
 * @param waveform waveform array data (32 bytes)
 */
void fcs80_scc_waveform_ch0_set(uint16_t waveform) __z88dk_fastcall;

/**
 * @brief SCC: Set Ch1 waveform
 * @param waveform waveform array data (32 bytes)
 */
void fcs80_scc_waveform_ch1_set(uint16_t waveform) __z88dk_fastcall;

/**
 * @brief SCC: Set Ch2 waveform
 * @param waveform waveform array data (32 bytes)
 */
void fcs80_scc_waveform_ch2_set(uint16_t waveform) __z88dk_fastcall;

/**
 * @brief SCC: Set Ch3 and 4 waveform
 * @param waveform waveform array data (32 bytes)
 */
void fcs80_scc_waveform_ch34_set(uint16_t waveform) __z88dk_fastcall;

/**
 * @brief SCC: Set waveform
 * @param ch channel (0-4)
 * @param waveform waveform array data (32 bytes)
 * @note ch4's waveform is save as ch3
 */
void fcs80_scc_waveform_set(uint8_t ch, const void* waveform) __smallc;

/**
 * @brief SCC: Get Ch0 waveform
 * @param waveform waveform array data (32 bytes buffer)
 */
void fcs80_scc_waveform_ch0_get(uint16_t waveform) __z88dk_fastcall;

/**
 * @brief SCC: Get Ch1 waveform
 * @param waveform waveform array data (32 bytes buffer)
 */
void fcs80_scc_waveform_ch1_get(uint16_t waveform) __z88dk_fastcall;

/**
 * @brief SCC: Get Ch2 waveform
 * @param waveform waveform array data (32 bytes buffer)
 */
void fcs80_scc_waveform_ch2_get(uint16_t waveform) __z88dk_fastcall;

/**
 * @brief SCC: Get Ch3 and Ch4 waveform
 * @param waveform waveform array data (32 bytes buffer)
 */
void fcs80_scc_waveform_ch34_get(uint16_t waveform) __z88dk_fastcall;

/**
 * @brief SCC: Get waveform
 * @param ch channel (0-4)
 * @param waveform waveform array data (32 bytes buffer)
 * @note ch4's waveform is save as ch3
 */
void fcs80_scc_waveform_get(uint8_t ch, void* waveform) __smallc;

/**
 * @brief SCC: Set Ch0 tone
 * @param tone 12bits tone (0-4095)
 * @note about tone: https://github.com/suzukiplan/fcs80/blob/master/README.md#register-05-tone-generator
 */
void fcs80_scc_tone_ch0_set(uint16_t tone) __z88dk_fastcall;

/**
 * @brief SCC: Set Ch1 tone
 * @param tone 12bits tone (0-4095)
 * @note about tone: https://github.com/suzukiplan/fcs80/blob/master/README.md#register-05-tone-generator
 */
void fcs80_scc_tone_ch1_set(uint16_t tone) __z88dk_fastcall;

/**
 * @brief SCC: Set Ch2 tone
 * @param tone 12bits tone (0-4095)
 * @note about tone: https://github.com/suzukiplan/fcs80/blob/master/README.md#register-05-tone-generator
 */
void fcs80_scc_tone_ch2_set(uint16_t tone) __z88dk_fastcall;

/**
 * @brief SCC: Set Ch3 tone
 * @param tone 12bits tone (0-4095)
 * @note about tone: https://github.com/suzukiplan/fcs80/blob/master/README.md#register-05-tone-generator
 */
void fcs80_scc_tone_ch3_set(uint16_t tone) __z88dk_fastcall;

/**
 * @brief SCC: Set Ch4 tone
 * @param tone 12bits tone (0-4095)
 * @note about tone: https://github.com/suzukiplan/fcs80/blob/master/README.md#register-05-tone-generator
 */
void fcs80_scc_tone_ch4_set(uint16_t tone) __z88dk_fastcall;

/**
 * @brief SCC: Set tone
 * @param ch channel (0-4)
 * @param tone 12bits tone (0-4095)
 * @note about tone: https://github.com/suzukiplan/fcs80/blob/master/README.md#register-05-tone-generator
 */
void fcs80_scc_tone_set(uint8_t ch, uint16_t tone) __smallc;

/**
 * @brief SCC: Set Ch0 volume
 * @param volume volume (0-15)
 */
void fcs80_scc_volume_ch0_set(uint8_t volume) __z88dk_fastcall;

/**
 * @brief SCC: Set Ch1 volume
 * @param volume volume (0-15)
 */
void fcs80_scc_volume_ch1_set(uint8_t volume) __z88dk_fastcall;

/**
 * @brief SCC: Set Ch2 volume
 * @param volume volume (0-15)
 */
void fcs80_scc_volume_ch2_set(uint8_t volume) __z88dk_fastcall;

/**
 * @brief SCC: Set Ch3 volume
 * @param volume volume (0-15)
 */
void fcs80_scc_volume_ch3_set(uint8_t volume) __z88dk_fastcall;

/**
 * @brief SCC: Set Ch4 volume
 * @param volume volume (0-15)
 */
void fcs80_scc_volume_ch4_set(uint8_t volume) __z88dk_fastcall;

/**
 * @brief SCC: Set volume
 * @param ch channel (0-4)
 * @param volume volume (0-15)
 */
void fcs80_scc_volume_set(uint8_t ch, uint8_t volume) __smallc;

/**
 * @brief SCC: Set mixing
 * @param mixing mixing
 */
void fcs80_scc_mixing_set(uint8_t mixing) __z88dk_fastcall;

/**
 * @brief VGS: Play BGM
 * @param song number of the song to play
 */
void fcs80_vgs_play(uint8_t song) __z88dk_fastcall;

/**
 * @brief VGS: Pause BGM
 */
void fcs80_vgs_pause() __z88dk_fastcall;

/**
 * @brief VGS: Resume BGM
 */
void fcs80_vgs_resume() __z88dk_fastcall;

/**
 * @brief VGS: Fadeout BGM
 */
void fcs80_vgs_fadeout() __z88dk_fastcall;

#endif
