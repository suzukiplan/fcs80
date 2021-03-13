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
    out ($C0), a

    ; 1画面分のマップデータ（32x24=768）をBGに描画
    ld bc, 768
    ld hl, $4000
    ld de, $8020
    ldir

    ; メインループ
mainloop:
    ; ラスタースクロール実行タイミング調整
    ld a, ($9600)           ; 現在のスキャンライン位置を取得
    cp a, $FF               ; blankチェック
    jp z, mainloop          ; blank中は何もしない
    ld b, a
    ld a, ($C000)           ; 直前のスキャンライン位置を取得
    cp a, b                 ; 現在のスキャンライン位置 と 直前のスキャンライン位置 が変わったかチェック
    jp z, mainloop          ; (変わっていなければ何もしない)
    ld a, b
    ld ($C000), a           ; 直前のスキャンライン位置情報 を 現在のスキャンライン位置 に更新しておく

    ; スクロール値特定
    ld a, ($C001)
    inc a
    and a, $3f
    ld ($C001), a
    srl a
    ld hl, scroll_values_0
    add l
    ld l, a
    ld a, h
    adc 0
    ld h, a

    ; BG Scroll Xを設定
    ld a, (hl)
    ld ($9602), a

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
scroll_values_0: defb 0, 1, 2, 3, 4, 5, 6, 7
scroll_values_1: defb 7, 6, 5, 4, 3, 2, 1, 0
scroll_values_2: defb 0, -1, -2, -3, -4, -5, -6, -7
scroll_values_3: defb -7, -6, -5, -4, -3, -2, -1, 0