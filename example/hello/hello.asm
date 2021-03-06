org $0000
.main
    ; 割り込み関連の初期化
    IM 1
    DI

    ; VBLANKを待機
    call wait_vblank

    ; パレットを初期化
    ld bc, 8
    ld hl, palette0_data
    ld de, $9400
    ldir

    ; Bank 1 を Character Pattern Table ($A000) に転送 (DMA)
    ld a, $01
    out ($c0), a

    ; 画面中央付近 (10,12) に "HELLO,WORLD!" を描画
    ld bc, 12
    ld hl, hello_text
    ld de, 394 + $8000 ; 394 = 12 * 32 + 10
    ldir

    ; 無限ループ
end_loop:
    jmp end_loop

; VBLANKになるまで待機
.wait_vblank
    ld hl, $9600
wait_vblank_loop:
    ld a, (hl)
    cp $ff
    jp nz, wait_vblank_loop
    ret

palette0_data: defw %0000000000000000, %0001110011100111, %0110001100011000, %0111111111111111
hello_text: defb "HELLO,WORLD!"