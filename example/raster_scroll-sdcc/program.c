#include "../../lib/sdcc/fcs80.h"

// ラスタースクロールの移動座標
const int8_t stbl[32] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    7, 6, 5, 4, 3, 2, 1, 0,
    0, -1, -2, -3, -4, -5, -6, -7,
    -7, -6, -5, -4, -3, -2, -1, 0
};

void main(void)
{
    // パレットを初期化
    fcs80_palette_set_rgb555(0, 0, 0b0000000000000000);
    fcs80_palette_set_rgb555(0, 1, 0b0001110011100111);
    fcs80_palette_set_rgb555(0, 2, 0b0110001100011000);
    fcs80_palette_set_rgb555(0, 3, 0b0111111111111111);
    fcs80_palette_set_rgb555(0, 4, 0b0000001110000000);
    fcs80_palette_set_rgb555(0, 5, 0b0000000000011100);

    // Bank 2 を Character Pattern Table ($A000) に転送 (DMA)
    fcs80_dma(2);

    // 1画面分のマップデータ（32x24=768）をBGに描画
    fcs80_memcpy(0x8020, 0x6000, 768);

    // ラスタースクロール値のテーブル
    uint8_t sidx = 0;

    // メインループ
    while (1) {
        fcs80_wait_vsync();
        // 1行づつスクロール値を設定
        for (uint8_t scanline = 0; scanline < 192; scanline++) {
            fcs80_wait_scanline(scanline);
            *FCS80_ADDR_BG_SCROLL_X = stbl[(scanline + sidx) & 0x1F];
        }
        sidx++;
    }
}