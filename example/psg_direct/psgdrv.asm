; AY-3-8910 Sound Driver for Z80
; Copyright 2021, SUZUKI PLAN (MIT License)
defc PSGDRV_RAM_HEAD = $C000    ; 使用するRAMの先頭アドレス（そのアドレスから 17バイト のワーク領域を使用する）
defc PSGDRV_SEQUENCE_POSITION = PSGDRV_RAM_HEAD + 0
defc PSGDRV_WAIT_COUNTER = PSGDRV_RAM_HEAD + 2
defc PSGDRV_VOL_DOWN_INTERVAL_CH0 = PSGDRV_RAM_HEAD + 3
defc PSGDRV_VOL_DOWN_INTERVAL_CH1 = PSGDRV_RAM_HEAD + 4
defc PSGDRV_VOL_DOWN_INTERVAL_CH2 = PSGDRV_RAM_HEAD + 5
defc PSGDRV_VOL_DOWN_COUNTER_CH0 = PSGDRV_RAM_HEAD + 6
defc PSGDRV_VOL_DOWN_COUNTER_CH1 = PSGDRV_RAM_HEAD + 7
defc PSGDRV_VOL_DOWN_COUNTER_CH2 = PSGDRV_RAM_HEAD + 8
defc PSGDRV_LOOP_MARK = PSGDRV_RAM_HEAD + 9
defc PSGDRV_PITCH_DOWN_CH0 = PSGDRV_RAM_HEAD + 11
defc PSGDRV_PITCH_DOWN_CH1 = PSGDRV_RAM_HEAD + 12
defc PSGDRV_PITCH_DOWN_CH2 = PSGDRV_RAM_HEAD + 13
defc PSGDRV_PITCH_UP_CH0 = PSGDRV_RAM_HEAD + 14
defc PSGDRV_PITCH_UP_CH1 = PSGDRV_RAM_HEAD + 15
defc PSGDRV_PITCH_UP_CH2 = PSGDRV_RAM_HEAD + 16

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
    in a, ($D8)
    and $0F
    jp z, psgdrv_execute_env1
    ld c, a
    ld a, (PSGDRV_VOL_DOWN_COUNTER_CH0)
    jp z, psgdrv_execute_env0_pd
    inc a
    ld (PSGDRV_VOL_DOWN_COUNTER_CH0), a
    cp a, b
    jp nz, psgdrv_execute_env0_pd
    ld a, 0
    ld (PSGDRV_VOL_DOWN_COUNTER_CH0), a
    ld a, c
    dec a
    out ($D8), a
psgdrv_execute_env0_pd:
    ld a, (PSGDRV_PITCH_DOWN_CH0)
    and $FF
    jp z, psgdrv_execute_env0_pu
    ld b, a
    in a, ($D0)
    add b
    out ($D0), a
    in a, ($D1)
    adc 0
    and $0F
    out ($D1), a
psgdrv_execute_env0_pu:
    ld a, (PSGDRV_PITCH_UP_CH0)
    and $FF
    jp z, psgdrv_execute_env1
    ld b, a
    in a, ($D0)
    sub b
    out ($D0), a
    in a, ($D1)
    sbc 0
    and $0F
    out ($D1), a
psgdrv_execute_env1:
    ; Ch1 のソフトウェアエンベロープ処理を実行
    ld a, (PSGDRV_VOL_DOWN_INTERVAL_CH1)
    ld b, a
    and $FF
    jp z, psgdrv_execute_env2
    in a, ($D9)
    and $0F
    jp z, psgdrv_execute_env2
    ld c, a
    ld a, (PSGDRV_VOL_DOWN_COUNTER_CH1)
    jp z, psgdrv_execute_env1_pd
    inc a
    ld (PSGDRV_VOL_DOWN_COUNTER_CH1), a
    cp a, b
    jp nz, psgdrv_execute_env1_pd
    ld a, 0
    ld (PSGDRV_VOL_DOWN_COUNTER_CH1), a
    ld a, c
    dec a
    out ($D9), a
psgdrv_execute_env1_pd:
    ld a, (PSGDRV_PITCH_DOWN_CH1)
    and $FF
    jp z, psgdrv_execute_env1_pu
    ld b, a
    in a, ($D2)
    add b
    out ($D2), a
    in a, ($D3)
    adc 0
    and $0F
    out ($D3), a
psgdrv_execute_env1_pu:
    ld a, (PSGDRV_PITCH_UP_CH1)
    and $FF
    jp z, psgdrv_execute_env2
    ld b, a
    in a, ($D2)
    sub b
    out ($D2), a
    in a, ($D3)
    sbc 0
    and $0F
    out ($D3), a
psgdrv_execute_env2:
    ; Ch2 のソフトウェアエンベロープ処理を実行
    ld a, (PSGDRV_VOL_DOWN_INTERVAL_CH2)
    ld b, a
    and $FF
    jp z, psgdrv_execute_env3
    in a, ($DA)
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
    out ($DA), a
psgdrv_execute_env2_pd:
    ld a, (PSGDRV_PITCH_DOWN_CH2)
    and $FF
    jp z, psgdrv_execute_env2_pu
    ld b, a
    in a, ($D4)
    add b
    out ($D4), a
    in a, ($D5)
    adc 0
    and $0F
    out ($D5), a
psgdrv_execute_env2_pu:
    ld a, (PSGDRV_PITCH_UP_CH2)
    and $FF
    jp z, psgdrv_execute_env3
    ld b, a
    in a, ($D4)
    sub b
    out ($D4), a
    in a, ($D5)
    sbc 0
    and $0F
    out ($D5), a
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
psgdrv_parse_tone0:
    ld b, a
    and $F0
    jp nz, psgdrv_parse_tone1
    call psgdrv_setup_tone_generator_Ch0
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_tone1:
    ld a, b
    and $F0
    cp $10
    jp nz, psgdrv_parse_tone2
    call psgdrv_setup_tone_generator_Ch1
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_tone2:
    ld a, b
    and $F0
    cp $20
    jp nz, psgdrv_parse_keyon0
    call psgdrv_setup_tone_generator_Ch2
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_keyon0:
    ld a, b
    and $F0
    cp $30
    jp nz, psgdrv_parse_keyon1
    call psgdrv_key_on_Ch0
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_keyon1:
    ld a, b
    and $F0
    cp $40
    jp nz, psgdrv_parse_keyon2
    call psgdrv_key_on_Ch1
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_keyon2:
    ld a, b
    and $F0
    cp $50
    jp nz, psgdrv_parse_senv0
    call psgdrv_key_on_Ch2
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_senv0:
    ld a, b
    cp $60
    jp nz, psgdrv_parse_senv1
    ld a, (hl)
    call psgdrv_increment_sequence_position
    ld (PSGDRV_VOL_DOWN_INTERVAL_CH0), a
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_senv1:
    ld a, b
    cp $61
    jp nz, psgdrv_parse_senv2
    ld a, (hl)
    call psgdrv_increment_sequence_position
    ld (PSGDRV_VOL_DOWN_INTERVAL_CH1), a
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_senv2:
    ld a, b
    cp $62
    jp nz, psgdrv_parse_wait
    ld a, (hl)
    call psgdrv_increment_sequence_position
    ld (PSGDRV_VOL_DOWN_INTERVAL_CH2), a
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_wait:
    ld a, b
    cp $63
    jp nz, psgdrv_parse_noise
    ld a, (hl)
    call psgdrv_increment_sequence_position
    ld (PSGDRV_WAIT_COUNTER), a
    ld a, $00 ; end sequence
    ret
psgdrv_parse_noise:
    ld a, b
    cp $64
    jp nz, psgdrv_parse_mixing
    call psgdrv_set_noise
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_mixing:
    ld a, b
    cp $65
    jp nz, psgdrv_parse_loop_mark
    call psgdrv_set_mixing
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_loop_mark:
    ld a, b
    cp $66
    jp nz, psgdrv_parse_loop_jump
    ld bc, (PSGDRV_SEQUENCE_POSITION)
    ld a, c
    ld (PSGDRV_LOOP_MARK), a
    ld a, b
    ld (PSGDRV_LOOP_MARK + 1), a
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_loop_jump:
    ld a, b
    cp $67
    jp nz, psgdrv_parse_pitch_down0
    ld hl, (PSGDRV_LOOP_MARK)
    ld a, l
    ld (PSGDRV_SEQUENCE_POSITION), a
    ld a, h
    ld (PSGDRV_SEQUENCE_POSITION + 1), a
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_pitch_down0:
    ld a, b
    cp $70
    jp nz, psgdrv_parse_pitch_down1
    ld a, (hl)
    call psgdrv_increment_sequence_position
    ld (PSGDRV_PITCH_DOWN_CH0), a
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_pitch_down1:
    ld a, b
    cp $71
    jp nz, psgdrv_parse_pitch_down2
    ld a, (hl)
    call psgdrv_increment_sequence_position
    ld (PSGDRV_PITCH_DOWN_CH1), a
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_pitch_down2:
    ld a, b
    cp $72
    jp nz, psgdrv_parse_pitch_up0
    ld a, (hl)
    call psgdrv_increment_sequence_position
    ld (PSGDRV_PITCH_DOWN_CH2), a
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_pitch_up0:
    ld a, b
    cp $73
    jp nz, psgdrv_parse_pitch_up1
    ld a, (hl)
    call psgdrv_increment_sequence_position
    ld (PSGDRV_PITCH_UP_CH0), a
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_pitch_up1:
    ld a, b
    cp $74
    jp nz, psgdrv_parse_pitch_up2
    ld a, (hl)
    call psgdrv_increment_sequence_position
    ld (PSGDRV_PITCH_UP_CH1), a
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_pitch_up2:
    ld a, b
    cp $75
    jp nz, psgdrv_parse_end
    ld a, (hl)
    call psgdrv_increment_sequence_position
    ld (PSGDRV_PITCH_UP_CH2), a
    ld a, $FF ; keep sequence
    ret
psgdrv_parse_end:
    ; シーケンス終了（シーケンス位置をデクリメントしてそこから先を参照しないようにする）
    ld hl, (PSGDRV_SEQUENCE_POSITION)
    dec hl
    ld (PSGDRV_SEQUENCE_POSITION), hl
    ld a, $00 ; end sequence
    ret

.psgdrv_setup_tone_generator_Ch0
    ld a, b
    and $0F
    out ($D1), a
    ld a, (hl)
    call psgdrv_increment_sequence_position
    out ($D0), a
    ret

.psgdrv_setup_tone_generator_Ch1
    ld a, b
    and $0F
    out ($D3), a
    ld a, (hl)
    call psgdrv_increment_sequence_position
    out ($D2), a
    ret

.psgdrv_setup_tone_generator_Ch2
    ld a, b
    and $0F
    out ($D5), a
    ld a, (hl)
    call psgdrv_increment_sequence_position
    out ($D4), a
    ret

.psgdrv_key_on_Ch0
    ld a, b
    and $0F
    out ($D8), a
    ld a, 0
    ld (PSGDRV_VOL_DOWN_COUNTER_CH0), a
    ret

.psgdrv_key_on_Ch1
    ld a, b
    and $0F
    out ($D9), a
    ld a, 0
    ld (PSGDRV_VOL_DOWN_COUNTER_CH1), a
    ret

.psgdrv_key_on_Ch2
    ld a, b
    and $0F
    out ($DA), a
    ld a, 0
    ld (PSGDRV_VOL_DOWN_COUNTER_CH2), a
    ret

.psgdrv_set_noise
    ld a, (hl)
    call psgdrv_increment_sequence_position
    out ($D6), a
    ret

.psgdrv_set_mixing
    ld a, (hl)
    call psgdrv_increment_sequence_position
    out ($D7), a
    ret
