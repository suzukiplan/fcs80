org $0000
.main
    ; 割り込み関連の初期化
    IM 1
    DI

    ; VBLANKを待機
    call wait_vblank

    ; パレットを初期化
    ld bc, 12
    ld hl, palette0_data
    ld de, $9400
    ldir

    ; Bank 1 を Character Pattern Table ($A000) に転送 (DMA)
    ld a, $01
    out ($c0), a

    ; マップの初期状態を描画
    ld bc, 800
    ld hl, $4000 + 8192 - 800
    ld a, l
    ld ($C002), a   ; Next read position (low)
    ld a, h
    ld ($C003), a   ; Next read position (high)
    ld de, $8000
    ldir
    ld a, $00
    ld ($C004), a   ; Next write nametbl (low)
    ld ($C005), a   ; Next write nametbl (high)

    ; メインループ
mainloop:
    ; VBLANKを待機
    call wait_vblank

    ; BGを下スクロール
    ld hl, $9603
    dec (hl)

    ; スクロールカウンタ (RAM) をインクリメント
    ld a, ($C000)
    inc a
    and $07
    ld ($C000), a
    jp nz, mainloop_next

    ; 次のマップを描画
    ld hl, ($C002)
    ld a, h
    cp $40
    jp z, mainloop_jump_to_head
    ld a, l
    sub $20
    ld l, a
    ld a, h
    sbc $00
    ld h, a
    ld ($C002), hl
    jmp mainloop_set_nametbl
mainloop_jump_to_head:
    ; 先頭に達したのでループさせる
    ld hl, $5FE0
    ld ($C002), hl
mainloop_set_nametbl:
    ld de, ($C004)
    ld a, e
    sub $20
    ld e, a
    ld a, d
    sbc $00
    and $03
    ld d, a
    ld ($C004), de
    ld a, d
    or $80
    ld d, a
    ld bc, 32
    ldir

mainloop_next:
    jmp mainloop

; VBLANKになるまで待機
.wait_vblank
    ld hl, $9607
wait_vblank_loop:
    ld a, (hl)
    and $80
    jp z, wait_vblank_loop
    ret

palette0_data: defw %0000000000000000, %0001110011100111, %0110001100011000, %0111111111111111, %0000001110000000, %0000000000011100
