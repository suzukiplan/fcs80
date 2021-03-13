; AY-3-8910 Sound Driver for Z80
; Copyright 2021, SUZUKI PLAN (MIT License)
defc PSGDRV_AY_LATCH = $A0      ; AY-3-8910 のレジスタ番号設定ポート番号
defc PSGDRV_AY_WRITE = $A1      ; AY-3-8910 のデータ書き込みポート番号
defc PSGDRV_AY_READ = $A2       ; AY-3-8910 のデータ読み込みポート番号
defc PSGDRV_RAM_HEAD = $C000    ; 使用するRAMの先頭アドレス（そのアドレスから 9バイト のワーク領域を使用する）
defc PSGDRV_SEQUENCE_POSITION = PSGDRV_RAM_HEAD + 0
defc PSGDRV_WAIT_COUNTER = PSGDRV_RAM_HEAD + 2
defc PSGDRV_VOL_DOWN_INTERVAL_CH0 = PSGDRV_RAM_HEAD + 3
defc PSGDRV_VOL_DOWN_INTERVAL_CH1 = PSGDRV_RAM_HEAD + 4
defc PSGDRV_VOL_DOWN_INTERVAL_CH2 = PSGDRV_RAM_HEAD + 5
defc PSGDRV_VOL_DOWN_COUNTER_CH0 = PSGDRV_RAM_HEAD + 6
defc PSGDRV_VOL_DOWN_COUNTER_CH1 = PSGDRV_RAM_HEAD + 7
defc PSGDRV_VOL_DOWN_COUNTER_CH2 = PSGDRV_RAM_HEAD + 8

; Z80 + AY-3-8910 用のシンプルなサウンドドライバ
; - psgdrv_execute を 1秒間に60回 呼び出せば OK
; - HL にシーケンスデータの先頭アドレスを指定する
; - レジスタの状態は保持されない（必要に応じて呼び出し元でpush/popする想定）
; - 裏レジスタは使用しない
; - シーケンスデータの仕様は README.md を参照
; - シーケンスデータはROM/RAMのどちらに配置してもOK (RAM展開せずにROMのデータを直接再生可能)
.psgdrv_execute
    ; Ch0 のソフトウェアエンベロープ処理を実行
    ld a, (PSGDRV_VOL_DOWN_INTERVAL_CH0)
    ld b, a
    and $FF
    jp z, psgdrv_execute_env1
    ld a, $08
    out (PSGDRV_AY_LATCH), a
    in a, (PSGDRV_AY_READ)
    and $0F
    jp z, psgdrv_execute_env1
    ld c, a
    ld a, (PSGDRV_VOL_DOWN_COUNTER_CH0)
    jp z, psgdrv_execute_env1
    inc a
    ld (PSGDRV_VOL_DOWN_COUNTER_CH0), a
    cp a, b
    jp nz, psgdrv_execute_env1
    ld a, 0
    ld (PSGDRV_VOL_DOWN_COUNTER_CH0), a
    ld a, c
    dec a
    out (PSGDRV_AY_WRITE), a
psgdrv_execute_env1:
    ; Ch1 のソフトウェアエンベロープ処理を実行
    ld a, (PSGDRV_VOL_DOWN_INTERVAL_CH1)
    ld b, a
    and $FF
    jp z, psgdrv_execute_env2
    ld a, $09
    out (PSGDRV_AY_LATCH), a
    in a, (PSGDRV_AY_READ)
    and $0F
    jp z, psgdrv_execute_env2
    ld c, a
    ld a, (PSGDRV_VOL_DOWN_COUNTER_CH1)
    jp z, psgdrv_execute_env2
    inc a
    ld (PSGDRV_VOL_DOWN_COUNTER_CH1), a
    cp a, b
    jp nz, psgdrv_execute_env2
    ld a, 0
    ld (PSGDRV_VOL_DOWN_COUNTER_CH1), a
    ld a, c
    dec a
    out (PSGDRV_AY_WRITE), a
psgdrv_execute_env2:
    ; Ch2 のソフトウェアエンベロープ処理を実行
    ld a, (PSGDRV_VOL_DOWN_INTERVAL_CH2)
    ld b, a
    and $FF
    jp z, psgdrv_execute_env3
    ld a, $0A
    out (PSGDRV_AY_LATCH), a
    in a, (PSGDRV_AY_READ)
    and $0F
    jp z, psgdrv_execute_env3
    ld c, a
    ld a, (PSGDRV_VOL_DOWN_COUNTER_CH2)
    jp z, psgdrv_execute_env3
    inc a
    ld (PSGDRV_VOL_DOWN_COUNTER_CH2), a
    cp a, b
    jp nz, psgdrv_execute_env3
    ld a, 0
    ld (PSGDRV_VOL_DOWN_COUNTER_CH2), a
    ld a, c
    dec a
    out (PSGDRV_AY_WRITE), a
psgdrv_execute_env3:
    ; ウェイトカウンタの減算
    ld a, (PSGDRV_WAIT_COUNTER)
    and $FF
    jp z, psgdrv_execute_1
    dec a
    ld (PSGDRV_WAIT_COUNTER), a
    ret
psgdrv_execute_1:
    ; シーケンス位置へのシーク
    ld bc, (PSGDRV_SEQUENCE_POSITION)
    ld a, c
    add l
    ld l, a
    ld a, h
    adc b
    ld h, a
    ; シーケンス処理ループ
psgdrv_execute_2:
    ld a, (hl)
    call psgdrv_increment_sequence_position
    call psgdrv_parse
    and $FF
    jp nz, psgdrv_execute_2
    ret

; シーケンス位置をインクリメント
.psgdrv_increment_sequence_position
    inc hl
    push hl
    ld hl, (PSGDRV_SEQUENCE_POSITION)
    inc hl
    ld (PSGDRV_SEQUENCE_POSITION), hl
    pop hl
    ret

; シーケンスデータのパース処理
.psgdrv_parse
    ld b, a
    and $F0
    jp nz, psgdrv_parse_1
    call psgdrv_setup_tone_generator_Ch0
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_1:
    ld a, b
    and $F0
    cp $10
    jp nz, psgdrv_parse_2
    call psgdrv_setup_tone_generator_Ch1
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_2:
    ld a, b
    and $F0
    cp $20
    jp nz, psgdrv_parse_3
    call psgdrv_setup_tone_generator_Ch2
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_3:
    ld a, b
    and $F0
    cp $30
    jp nz, psgdrv_parse_4
    call psgdrv_key_on_Ch0
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_4:
    ld a, b
    and $F0
    cp $40
    jp nz, psgdrv_parse_5
    call psgdrv_key_on_Ch1
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_5:
    ld a, b
    and $F0
    cp $50
    jp nz, psgdrv_parse_6
    call psgdrv_key_on_Ch2
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_6:
    ld a, b
    cp $60
    jp nz, psgdrv_parse_7
    ld a, (hl)
    call psgdrv_increment_sequence_position
    ld (PSGDRV_VOL_DOWN_INTERVAL_CH0), a
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_7:
    ld a, b
    cp $61
    jp nz, psgdrv_parse_8
    ld a, (hl)
    call psgdrv_increment_sequence_position
    ld (PSGDRV_VOL_DOWN_INTERVAL_CH1), a
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_8:
    ld a, b
    cp $62
    jp nz, psgdrv_parse_9
    ld a, (hl)
    call psgdrv_increment_sequence_position
    ld (PSGDRV_VOL_DOWN_INTERVAL_CH2), a
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_9:
    ld a, b
    cp $63
    jp nz, psgdrv_parse_10
    ld a, (hl)
    call psgdrv_increment_sequence_position
    ld (PSGDRV_WAIT_COUNTER), a
    ld a, $00 ; end sequence
    ret
psgdrv_parse_10:
    ld a, b
    cp $64
    jp nz, psgdrv_parse_11
    call psgdrv_set_noise
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_11:
    ld a, b
    cp $65
    jp nz, psgdrv_parse_12
    call psgdrv_set_mixing
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_12:
    ; シーケンス終了（シーケンス位置をデクリメントしてそこから先を参照しないようにする）
    ld hl, (PSGDRV_SEQUENCE_POSITION)
    dec hl
    ld (PSGDRV_SEQUENCE_POSITION), hl
    ld a, $00 ; end sequence
    ret

.psgdrv_setup_tone_generator_Ch0
    ld a, $01
    out (PSGDRV_AY_LATCH), a
    ld a, b
    and $0F
    out (PSGDRV_AY_WRITE), a
    ld a, $00
    out (PSGDRV_AY_LATCH), a
    ld a, (hl)
    call psgdrv_increment_sequence_position
    out (PSGDRV_AY_WRITE), a
    ret

.psgdrv_setup_tone_generator_Ch1
    ld a, $03
    out (PSGDRV_AY_LATCH), a
    ld a, b
    and $0F
    out (PSGDRV_AY_WRITE), a
    ld a, $02
    out (PSGDRV_AY_LATCH), a
    ld a, (hl)
    call psgdrv_increment_sequence_position
    out (PSGDRV_AY_WRITE), a
    ret

.psgdrv_setup_tone_generator_Ch2
    ld a, $05
    out (PSGDRV_AY_LATCH), a
    ld a, b
    and $0F
    out (PSGDRV_AY_WRITE), a
    ld a, $04
    out (PSGDRV_AY_LATCH), a
    ld a, (hl)
    call psgdrv_increment_sequence_position
    out (PSGDRV_AY_WRITE), a
    ret

.psgdrv_key_on_Ch0
    ld a, $08
    out (PSGDRV_AY_LATCH), a
    ld a, b
    and $0F
    out (PSGDRV_AY_WRITE), a
    ld a, 0
    ld (PSGDRV_VOL_DOWN_COUNTER_CH0), a
    ret

.psgdrv_key_on_Ch1
    ld a, $09
    out (PSGDRV_AY_LATCH), a
    ld a, b
    and $0F
    out (PSGDRV_AY_WRITE), a
    ld a, 0
    ld (PSGDRV_VOL_DOWN_COUNTER_CH1), a
    ret

.psgdrv_key_on_Ch2
    ld a, $0A
    out (PSGDRV_AY_LATCH), a
    ld a, b
    and $0F
    out (PSGDRV_AY_WRITE), a
    ld a, 0
    ld (PSGDRV_VOL_DOWN_COUNTER_CH2), a
    ret

.psgdrv_set_noise
    ld a, $06
    out (PSGDRV_AY_LATCH), a
    ld a, (hl)
    call psgdrv_increment_sequence_position
    out (PSGDRV_AY_WRITE), a
    ret

.psgdrv_set_mixing
    ld a, $07
    out (PSGDRV_AY_LATCH), a
    ld a, (hl)
    call psgdrv_increment_sequence_position
    out (PSGDRV_AY_WRITE), a
    ret
