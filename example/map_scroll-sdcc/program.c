#include "../../lib/sdcc/fcs80.h"

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

    // マップの初期状態を描画
    uint8_t scrollCounter = 0;
    uint16_t nextRead = 0x8000 - 32 * 25;
    int8_t nextWrite = 0;
    NameTable* bg = fcs80_bg_nametbl_get();
    fcs80_memcpy((uint16_t)bg, nextRead, 32 * 25);

    // メインループ
    while (1) {
        // VBLANKを待機
        fcs80_wait_vsync();

        // BGを下スクロール
        fcs80_bg_scroll_y(fcs80_bg_scroll_y_get() - 1);

        // スクロールカウンタをインクリメント
        scrollCounter++;
        scrollCounter &= 0x07;
        if (0 != scrollCounter) {
            continue;
        }

        // 次のマップを描画
        nextRead -= 32;
        nextRead &= 0x1FFF;
        nextRead |= 0x6000;
        nextWrite--;
        nextWrite &= 0x1F;
        fcs80_memcpy((uint16_t)&bg->ptn[nextWrite][0], nextRead, 32);
    }
}