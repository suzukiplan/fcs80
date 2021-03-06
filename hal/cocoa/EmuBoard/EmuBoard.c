//
//  EmuBoard.c
//  EmuBoard
//
//  Created by 鈴木　洋司　 on 2018/12/30.
//  Copyright © 2018年 SUZUKIPLAN. All rights reserved.
//

#include "EmuBoard.h"
#include "fcs80gw.h"
#include "vgsspu_al.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void* emu_fcs80;
unsigned short emu_vram[VRAM_WIDTH * VRAM_HEIGHT];
unsigned char emu_key = 0;
static void* spu;
pthread_mutex_t sound_locker;
static short sound_buffer[65536 * 2];
static volatile unsigned short sound_cursor;
static int emu_initialized = 0;

static void sound_callback(void* buffer, size_t size)
{
    while (sound_cursor < size / 2) usleep(100);
    pthread_mutex_lock(&sound_locker);
    memcpy(buffer, sound_buffer, size);
    if (size <= sound_cursor)
        sound_cursor -= size;
    else
        sound_cursor = 0;
    pthread_mutex_unlock(&sound_locker);
}

/**
 * 起動時に1回だけ呼び出される
 */
void emu_init(const void* rom, size_t romSize)
{
    puts("emu_init");
    if (emu_initialized) return;
    printf("load rom (size: %lu)\n", romSize);
    emu_fcs80 = fcs80_create();
    fcs80_loadRom(emu_fcs80, rom, romSize);
    pthread_mutex_init(&sound_locker, NULL);
    spu = vgsspu_start2(44100, 16, 2, 23520, sound_callback);
    emu_initialized = 1;
}

void emu_reload(const void* rom, size_t romSize)
{
    if (!emu_initialized) {
        emu_init(rom, romSize);
        return;
    }
    puts("emu_reload");
    if (emu_fcs80) fcs80_destroy(emu_fcs80);
    emu_fcs80 = fcs80_create();
    fcs80_loadRom(emu_fcs80, rom, romSize);
}

void emu_reset()
{
    fcs80_reset(emu_fcs80);
}

/**
 * 画面の更新間隔（1秒間で60回）毎にこの関数がコールバックされる
 * この中で以下の処理を実行する想定:
 * 1. エミュレータのCPU処理を1フレーム分実行
 * 2. エミュレータの画面バッファの内容をvramへコピー
 */
void emu_vsync()
{
    if (!emu_initialized || !emu_fcs80) return;
    fcs80_tick(emu_fcs80, emu_key, 0);
    const void* ptr = fcs80_display(emu_fcs80);
    memcpy(emu_vram, ptr, sizeof(emu_vram));
    size_t pcmSize;
    void* pcm = fcs80_sound(emu_fcs80, &pcmSize);
    pthread_mutex_lock(&sound_locker);
    if (pcmSize + sound_cursor < sizeof(sound_buffer)) {
        memcpy(&sound_buffer[sound_cursor], pcm, pcmSize);
        sound_cursor += pcmSize / 2;
    }
    pthread_mutex_unlock(&sound_locker);
}

unsigned int emu_getScore()
{
    if (!emu_initialized) return 0;
    return 0;
}

int emu_isGameOver()
{
    if (!emu_initialized) return 0;
    return 0;
}

/**
 * 終了時に1回だけ呼び出される
 * この中でエミュレータの初期化処理を実行する想定
 */
void emu_destroy()
{
    puts("emu_destroy");
    if (!emu_initialized) return;
    if (emu_fcs80) {
        fcs80_destroy(emu_fcs80);
        emu_fcs80 = NULL;
    }
    emu_initialized = 0;
}

const void* emu_saveState(size_t* size)
{
    static void* buffer;
    if (!emu_initialized || !emu_fcs80) return NULL;
    *size = fcs80_stateSize(emu_fcs80);
    if (!buffer) buffer = malloc(*size);
    fcs80_stateSave(emu_fcs80, buffer);
    return buffer;
}

void emu_loadState(const void* state, size_t size)
{
    if (!emu_initialized || !emu_fcs80) return;
    if (size != fcs80_stateSize(emu_fcs80)) return;
    fcs80_stateLoad(emu_fcs80, state);
}

void emu_dump()
{
    char buf[256];
    int i, j, ptr;
    const unsigned char* vram = fcs80_getVRAM(emu_fcs80);
    puts("[BG NameTable]");
    buf[0] = 0;
    ptr = 0;
    puts("ADDR: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F");
    puts("----: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --");
    for (i = 0; i < 32; i++) {
        sprintf(buf, "%04X:", ptr);
        for (j = 0; j < 32; j++) {
            sprintf(&buf[strlen(buf)], " %02X", vram[ptr++]);
        }
        printf("%s\n", buf);
    }
}
