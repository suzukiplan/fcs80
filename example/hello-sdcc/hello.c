#include "../../lib/sdcc/fcs80.h"

void main(void)
{
    uint8_t scrollX = 0;
    uint8_t scrollY = 0;
    uint8_t pad;

    // パレットを初期化
    fcs80_palette_set(0, 0, 0, 0, 0);    // black
    fcs80_palette_set(0, 1, 7, 7, 7);    // dark gray
    fcs80_palette_set(0, 2, 24, 24, 24); // light gray
    fcs80_palette_set(0, 3, 31, 31, 31); // white

    // Bank 2 を Character Pattern Table ($A000) に転送 (DMA)
    fcs80_dma(2);

    // 画面中央付近 (10,12) に "HELLO,WORLD!" を描画
    fcs80_bg_putstr(10, 12, 0x80, "HELLO,WORLD!");

    while (1) {
        fcs80_wait_vsync();
        pad = fcs80_joypad_get();
        if (pad & FCS80_JOYPAD_LE) {
            scrollX++;
            fcs80_bg_scroll_x(scrollX);
        } else if (pad & FCS80_JOYPAD_RI) {
            scrollX--;
            fcs80_bg_scroll_x(scrollX);
        }
        if (pad & FCS80_JOYPAD_UP) {
            scrollY++;
            fcs80_bg_scroll_y(scrollY);
        } else if (pad & FCS80_JOYPAD_DW) {
            scrollY--;
            fcs80_bg_scroll_y(scrollY);
        }
    }
}
