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

    ; FGの左上に "SPRITE TEST" を描画
    ld bc, 11
    ld hl, spriteTest_text
    ld de, $8842
    ldir
    ld bc, 11
    ld hl, spriteTest_attr
    ld de, $8C42
    ldir

    ; BG全画面にひし形を描画
    ld de, $8000
    ld b, $20
main_drawBgPtn:
    push b
    ld bc, $20
    ld hl, bgPtn_line
    ldir
    pop b
    djnz main_drawBgPtn

    ; BG全画面にひし形にするための反転アトリビュートを設定
    ld de, $8400
    ld b, $10
main_drawBgAttr:
    push b
    ld bc, $20
    ld hl, bgAttr_line0
    ldir
    ld bc, $20
    ld hl, bgAttr_line1
    ldir
    pop b
    djnz main_drawBgAttr

    ; OAMとスプライト制御変数の初期値を設定
    ld b, $00
    ld hl, $9000 ; OAM
    ld de, $C000 ; スプライト制御変数 (8 x 256)
main_initOAM:
    call get_random_8_184
    ld (hl), a ; Y座標 (8 ~ 184)
    inc hl
    call get_random_8_240
    ld (hl), a ; X座標 (8 ~ 240)
    inc hl
    call get_random_1_9
    ld (hl), a ; パターン (1 ~ 9)

    sla a
    sla a
    ld (de), a ; [0] アニメーションフラグ保持
    call get_random
    and $03
    inc de
    ld (de), a ; [1] 進行方向フラグ保持 ------vh
    call get_random
    or $0F
    inc de
    ld (de), a ; [2] 上下方向移動(wait)
    ld a, 0
    inc de
    ld (de), a ; [3] 上下方向移動(value)
    call get_random
    or $0F
    inc de
    ld (de), a ; [4] 左右方向移動(wait)
    ld a, 0
    inc de
    ld (de), a ; [5] 左右方向移動(value)

    inc hl
    ld (hl), $80 ; 属性
    inc hl
    add de, 3
    djnz main_initOAM

    ; メインループ
mainloop:
    ; VBLANKを待機
    call wait_vblank

    ; BGを下スクロール
    ld hl, $9603
    dec (hl)

    ; スプライト（256個）を動かす
    ld de, $9000
    ld hl, $C000
    ld b, $00
mainloop_drawSprites:
    call move_sprite
    add de, 4
    add hl, 8
    djnz mainloop_drawSprites

    jmp mainloop

; VBLANKになるまで待機
.wait_vblank
    ld hl, $9607
wait_vblank_loop:
    ld a, (hl)
    and $80
    jp z, wait_vblank_loop
    ret

; OAM(DE)のスプライトをRAM(HL)の内容に応じて動かす
.move_sprite
    push b
    push de
    push hl

    ; アニメーション（4フレームに1回）
    inc (hl)
    ld a, (hl)
    and %00011100
    srl a
    srl a
    inc a
    push de
    add de, 2
    ld (de), a
    pop de
    inc hl

    ; 移動方向情報をレジスタBに退避
    ld a, (hl)
    ld b, a

    ; 上下移動
    inc hl
    ld a, (hl)
    inc hl
    ld c, (hl)
    add c
    ld (hl), a
    jp nc, move_sprite_end_ud
    ld a, b
    and %00000010
    jp nz, move_sprite_up
    ld a, (de)
    inc a
    ld (de), a
    jmp move_sprite_end_ud
move_sprite_up:
    ld a, (de)
    dec a
    ld (de), a
move_sprite_end_ud:
    inc de

    ; 左右移動
    inc hl
    ld a, (hl)
    inc hl
    ld c, (hl)
    add c
    ld (hl), a
    jp nc, move_sprite_end_lr
    ld a, b
    and %00000001
    jp nz, move_sprite_left
    ld a, (de)
    inc a
    ld (de), a
    jmp move_sprite_end_lr
move_sprite_left:
    ld a, (de)
    dec a
    ld (de), a
move_sprite_end_lr:

    pop hl
    pop de
    pop b
    ret

; 0 ~ 255 の範囲の乱数をテーブルから取得してレジスタAに格納
.get_random
    push bc
    push hl
    ld hl, $D000
    ld a, (hl)
    inc (hl)
    ld bc, random_table
    add c
    ld c, a
    ld a, b
    adc 0
    ld b, a
    ld a, (bc)
    pop hl
    pop bc
    ret

; 8 ~ 184 の範囲の乱数を取得してレジスタAに格納
.get_random_8_184
    push b
    call get_random
    and $7F
    ld b, a ; b = 0 ~ 127
    call get_random
    and %00011111
    add b  ; b = 0 ~ 127 + 31 (158)
    ld b, a
    call get_random
    and %00001111  ; b = 0 ~ 127 + 31 + 15 (173)
    add b
    ld b, a
    call get_random
    and %00000011  ; b = 0 ~ 127 + 31 + 15 + 3 (176)
    add b
    add 8
    pop b
    ret

; 8 ~ 240 の範囲の乱数を取得してレジスタAに格納
.get_random_8_240
    push b
    call get_random
    and $7F
    ld b, a ; b = 0 ~ 127
    call get_random
    and %00111111
    add b  ; b = 0 ~ 127 + 63 (191)
    ld b, a
    call get_random
    and %00011111  ; b = 0 ~ 191 + 31 (222)
    add b
    ld b, a
    call get_random
    and %00000111  ; b = 0 ~ 222 + 7 (229)
    add b
    ld b, a
    call get_random
    and %00000011  ; b = 0 ~ 229 + 3 (232)
    add b
    add a, 8
    pop b
    ret

; 1 ~ 9 の範囲の乱数を取得してレジスタAに格納
.get_random_1_9
    call get_random
    and %00000111
    inc a
    ret

palette0_data: defw %0000000000000000, %0001110011100111, %0110001100011000, %0111111111111111, %0000001110000000, %0000000000011100
spriteTest_text: defb "SPRITE TEST"
spriteTest_attr: defb $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80
bgPtn_line: defb $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10, $10
bgAttr_line0: defb $00, $20, $00, $20, $00, $20, $00, $20, $00, $20, $00, $20, $00, $20, $00, $20, $00, $20, $00, $20, $00, $20, $00, $20, $00, $20, $00, $20, $00, $20, $00, $20
bgAttr_line1: defb $40, $60, $40, $60, $40, $60, $40, $60, $40, $60, $40, $60, $40, $60, $40, $60, $40, $60, $40, $60, $40, $60, $40, $60, $40, $60, $40, $60, $40, $60, $40, $60
random_table: defb $2A, $46, $5E, $7D, $45, $DA, $1E, $72, $38, $43, $D4, $D1, $3E, $69, $AC, $7E, $08, $79, $8F, $F5, $0F, $E8, $E4, $41, $6D, $71, $2F, $04, $E3, $5D, $D0, $C3, $19, $7B, $DF, $1A, $6E, $CD, $C8, $84, $27, $CA, $BA, $53, $A8, $62, $16, $FF, $3C, $22, $51, $95, $0E, $63, $26, $B3, $42, $ED, $A0, $78, $73, $C5, $34, $DE, $9F, $E6, $A1, $B9, $61, $59, $24, $9D, $F4, $68, $00, $5A, $7C, $91, $85, $C4, $D5, $3D, $C2, $31, $99, $30, $17, $8E, $3A, $96, $B7, $C1, $B1, $B5, $3B, $93, $EB, $4F, $4A, $9A, $70, $37, $60, $09, $D2, $AA, $D8, $B2, $D3, $29, $F7, $67, $1D, $0D, $F9, $4D, $F6, $77, $EC, $82, $06, $2B, $14, $F3, $6F, $F1, $4E, $BD, $83, $AF, $55, $81, $49, $6A, $50, $35, $A3, $E1, $8D, $75, $BC, $A9, $07, $65, $01, $57, $97, $E5, $C9, $3F, $10, $C0, $89, $EE, $74, $9E, $66, $8B, $0C, $1F, $25, $39, $64, $E2, $5C, $47, $40, $32, $FE, $6C, $F8, $B4, $A5, $B0, $44, $36, $CE, $5F, $6B, $05, $D7, $AE, $33, $52, $1B, $11, $1C, $DC, $48, $02, $CF, $F0, $80, $7F, $28, $E7, $92, $E0, $9B, $86, $20, $CB, $7A, $54, $0B, $C6, $94, $BF, $76, $DD, $CC, $B8, $13, $4B, $0A, $5B, $88, $FD, $18, $FA, $9C, $98, $A4, $2C, $DB, $12, $AD, $03, $58, $EF, $FB, $A6, $D6, $8C, $D9, $C7, $2D, $F2, $15, $A2, $2E, $A7, $4C, $87, $B6, $90, $56, $E9, $EA, $23, $BE, $FC, $AB, $8A, $21, $BB
