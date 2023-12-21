#ifndef INCLUDE_FCS80_H
#define INCLUDE_FCS80_H

#include <stdint.h>

#define FCS80_JOYPAD_UP 0b10000000
#define FCS80_JOYPAD_DW 0b01000000
#define FCS80_JOYPAD_LE 0b00100000
#define FCS80_JOYPAD_RI 0b00010000
#define FCS80_JOYPAD_ST 0b00001000
#define FCS80_JOYPAD_SE 0b00000100
#define FCS80_JOYPAD_T1 0b00000010
#define FCS80_JOYPAD_T2 0b00000001

void fcs80_wait_vsync(void);
void fcs80_palette_set(uint8_t pn, uint8_t pi, uint8_t r, uint8_t g, uint8_t b);
void fcs80_dma(uint8_t prg);
void fcs80_bg_putstr(uint8_t x, uint8_t y, uint8_t attr, const char* str);
uint8_t fcs80_joypad_get(void);
void fcs80_scroll_x(uint8_t x);
void fcs80_scroll_y(uint8_t y);

#endif
