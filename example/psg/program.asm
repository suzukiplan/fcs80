org $0000
.main
    ; 割り込み関連の初期化
    IM 1
    DI

    ; VBLANKを待機
    call wait_vblank

    ; メインループ
mainloop:
    ld hl, data_00
    call psgdrv_execute
    call wait_vblank
    jmp mainloop

; VBLANKになるまで待機
.wait_vblank
    ld hl, $9607
wait_vblank_loop:
    ld a, (hl)
    and $80
    jp z, wait_vblank_loop
    ret

include "psgdrv.asm"

; シーケンスデータ（きらきら星）
data_00: defb $65, %10111000 ; setup mixing (noise disable & tone enable)
data_01: defb $60, $04, $61, $02 ; setup software evelope
data_st: defb $66 ; loop mark
data_02: defb $01, $AC, $3F, $13, $57, $4F, $63, $10, $12, $3B, $4F, $63, $10, $3F, $12, $A7, $4F, $63, $10, $12, $3B, $4F, $63, $10 ; Ch0: c4 c4, Ch1: c8 g8 e8 g8
data_03: defb $01, $1D, $3F, $13, $57, $4F, $63, $10, $12, $3B, $4F, $63, $10, $3F, $12, $A7, $4F, $63, $10, $12, $3B, $4F, $63, $10 ; Ch0: g4 g4, Ch1: c8 g8 e8 g8
data_04: defb $00, $FE, $3F, $13, $57, $4F, $63, $10, $11, $FD, $4F, $63, $10, $3F, $12, $81, $4F, $63, $10, $11, $FD, $4F, $63, $10 ; Ch0: a4 a4, Ch1: c8 a8 f8 a8
data_05: defb $01, $1D, $3F, $13, $57, $4F, $63, $10, $12, $3B, $4F, $63, $10, $12, $A7, $4F, $63, $10, $12, $3B, $4F, $63, $10      ; Ch0: g4 r4, Ch1: c8 g8 e8 g8
data_06: defb $01, $40, $3F, $13, $8A, $4F, $63, $10, $12, $3B, $4F, $63, $10, $3F, $12, $FA, $4F, $63, $10, $12, $3B, $4F, $63, $10 ; Ch0: f4 f4, Ch1: b8 g8 d8 g8
data_07: defb $01, $53, $3F, $13, $57, $4F, $63, $10, $12, $3B, $4F, $63, $10, $3F, $12, $A7, $4F, $63, $10, $12, $3B, $4F, $63, $10 ; Ch0: e4 e4, Ch1: c8 g8 e8 g8
data_08: defb $01, $7D, $3F, $13, $8A, $4F, $63, $10, $12, $3B, $4F, $63, $10, $3F, $12, $FA, $4F, $63, $10, $12, $3B, $4F, $63, $10 ; Ch0: d4 d4, Ch1: b8 g8 d8 g8
data_09: defb $01, $AC, $3F, $13, $57, $4F, $63, $10, $12, $3B, $4F, $63, $10, $12, $A7, $4F, $63, $10, $12, $3B, $4F, $63, $10      ; Ch0: c4 r4, Ch1: c8 g8 e8 g8
data_ed: defb $67 ; Jump to loop mark
