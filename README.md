![fcs80-logo.png](fcs80-logo.png)

FAIRY COMPUTER SYSTEM 80 (FCS80) is a new retro game machine equipped with the Z80 and AY-3-8910, the actual technology of the 8-bit era, and VDP (FCS80-VIDEO), which has an easy-to-use interface with the Z80.
This repository provides an official emulator for the FCS80 core system.

## Table of content

- [1. Specification](#1-specification)
- [2. Memory Map](#2-memory-map)
  - [2-1. CPU Memory](#2-1-cpu-memory)
  - [2-2. VRAM](#2-2-vram)
  - [2-3. SCC](#2-3-scc)
  - [2-4. RAM](#2-4-ram)
- [3. I/O map](#3-io-map)
  - [3-1. $A0~$A2 or $D0~$DF: AY-3-8910](#3-1-a0a2-or-d0df-ay-3-8910)
  - [3-2. $B0~$B3: Bank switch](#3-2-b0b3-bank-switch)
  - [3-3. $C0: High Speed DMA (Bank to VRAM)](#3-3-c0-high-speed-dma-bank-to-vram)
  - [3-4. $C1: CPU boost flag](#3-4-c1-cpu-boost-flag)
  - [3-5. $C2: memset](#3-5-c2-memset)
  - [3-6. $C3: memcpy](#3-6-c3-memcpy)
- [4. ROM](#4-rom)
  - [4-1. File Format](#4-1-file-format)
- [5. Programming Guide](#5-programming-guide)
  - [5-1. Make your development environment](#5-1-make-your-development-environment)
  - [5-2. Example](#5-2-example)
  - [5-3. Notes for keeping the compatible](#5-3-notes-for-keeping-the-compatible)
  - [5-4. Toolchain](#5-4-toolchain)
- [6. HAL Implementation Guide](#6-hal-implementation-guide)
  - [6-1. Integrate it into your project](#6-1-integrate-it-into-your-project)
  - [6-2. How to use the FCS80 emulator](#6-2-how-to-use-the-fcs80-emulator)
  - [6-3. Example](#6-3-example)

## 1. Specification

- CPU: Z80A (3579545Hz)
- VDP: FCS80-VDP (4024320Hz)
  - Screen Resolution: 240 x 192
  - Background & Foreground 8x8 tiled graphics (both virtual screen: 256 x 256)
  - 256 sprites (size: 8x8) & max horizontal limit: 256 (unlimited)
  - Number of Scanlines: 262
  - Frame Rate: 60 frames per second
  - Colors: 256 colors in 65536 colors
- APU: AY-3-8910 (44100Hz)
- RAM: 16KB
- VRAM: 16KB

## 2. Memory Map

The FCS80 allocates the upper 32KB of the Z80's memory space (64KB) to the program and the lower 32KB to the random access memory map.

- The program (upper 32KB) is divided into four banks of 8KB each.
  - The program area is read-only.
  - Bank switching is possible by [3-2. $B0~$B3: Bank switch](#3-2-b0b3-bank-switch).
- In the random access memory map (lower 32KB), the upper 16KB is allocated to VRAM and the lower 16KB to RAM.
  - The memory map can be read and written.
  - There is no mechanism to switch banks of RAM.

### 2-1. CPU Memory

|    Address    | Map                                 |
| :-----------: | :---------------------------------- |
| $0000 ~ $1FFF | PRG0: ROM Bank #0 (Program)         |
| $2000 ~ $3FFF | PRG1: ROM Bank #1 (Program or Data) |
| $4000 ~ $5FFF | PRG2: ROM Bank #2 (Program or Data) |
| $6000 ~ $7FFF | PRG3: ROM Bank #3 (Program or Data) |
| $8000 ~ $97FF | VRAM (Name Table, OAM, Registers)   |
| $9800 ~ $98FF | SCC                                 |
| $A000 ~ $BFFF | VRAM (Character Pattern)            |
| $C000 ~ $FFFF | RAM (16KB)                          |

#### Discussion about there is no SRAM

The FCS80 does not have Static-RAM (also known as battery backup).
This is based on the idea that saves and loads are satisfied by state saves and state loads.
There may be cases where you want to keep high scores locally, but for leaderboards, I believe it is preferable to get them from outside the FCS80 (by reading RAM, etc.) and implement them outside the game.

#### Discussion about RAM size (16KB)

In today's programming environment, it is impossible to develop programs properly with only 16KB of RAM, but in the 8-bit era, 2KB of RAM was the norm for targeted game consoles.
Therefore, 16KB is more than enough memory for game development.
This memory size is sufficient for full assembly language and even C language game development.

### 2-2. VRAM

|  CPU address  | VRAM address  | Map                                                     |
| :-----------: | :-----------: | :------------------------------------------------------ |
| $8000 ~ $83FF | $0000 ~ $03FF | BG Name Table (32 x 32)                                 |
| $8400 ~ $87FF | $0400 ~ $07FF | BG Attribute Table (32 x 32)                            |
| $8800 ~ $8BFF | $0800 ~ $0BFF | FG Name Table (32 x 32)                                 |
| $8C00 ~ $8FFF | $0C00 ~ $0FFF | FG Attribute Table (32 x 32)                            |
| $9000 ~ $93FF | $1000 ~ $13FF | OAM; Object Attribute Memory (4 x 256)                  |
| $9400 ~ $95FF | $1400 ~ $15FF | Palette Table (2 x 16 x 16)                             |
|     $9600     |     $1600     | Register #0: Scanline vertical counter (read only)      |
|     $9601     |     $1601     | Register #1: Scanline horizontal counter (read only)    |
|     $9602     |     $1602     | Register #2: BG Scroll X                                |
|     $9603     |     $1603     | Register #3: BG Scroll Y                                |
|     $9604     |     $1604     | Register #4: FG Scroll X                                |
|     $9605     |     $1605     | Register #5: FG Scroll Y                                |
|     $9606     |     $1606     | Register #6: IRQ scanline position (NOTE: 0 is disable) |
|     $9607     |     $1607     | Register #7: Status (read only)                         |
| $A000 ~ $BFFF | $2000 ~ $3FFF | Character Pattern Table (32 x 256)                      |

#### VRAM access

In the case of typical 8-bit console VDPs (such as the PPU of the Family Computer and the VDP of the MSX), it is common to set the VRAM access address by writing a value twice to the address setting port, and then access it via the write and read ports.

In FCS80-VIDEO, since all addresses of VRAM are mapped to CPU memory, it is possible to write and read VRAM with instructions such as LD and LDIR, just as when accessing RAM.

#### Graphic Layered Structure

| Priority | Name                    | Description                                                                                                                                                           |
| :------: | :---------------------- | :-------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
|    1     | FG; Foreground Graphics | Graphics to be drawn at the highest priority. It is intended for use in drawing game screen frames, displaying scores, etc.                                           |
|    2     | Sprites                 | These graphics are designed to be used for drawing characters that move around on the screen. Unlike BG and FG, drawing data is managed by OAM instead of Name Table. |
|    3     | BG; Background Graphics | This is the graphics with the lowest drawing priority, intended for drawing the background of the game screen.                                                        |

#### Name Table & Scroll (BG/FG)

- 32 x 32 characters = 256 x 256 pixels
- masked the edge 8px of left, right and top
- masked the bottom edge 56px
- scrollable with Scroll Register (#2 ~ #5)

#### Attribute bit-layout (BG/FG/Sprite)

| Bit-7 | Bit-6 | Bit-5 | Bit-4 | Bit-3 | Bit-2 | Bit-1 | Bit-0 |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
|  VI   |  UD   |  LR   |   -   |  P3   |  P2   |  P1   |  P0   |

- VI: 0 = Hidden, 1 = Visible (NOTE: ignored if BG and always as Visible)
- UD: 1 = Flipping pattern upside down
- LR: 1 = Flipping pattern left to right
- P0 ~ P3: palette number (0 ~ 15)

#### Object Attribute Memory (Sprite)

```c
struct OAM {
    unsigned char y;
    unsigned char x;
    unsigned char pattern;
    unsigned char attribute;
} oam[256];
```

#### Palette Table

- FCS80 has 16 palettes
- 1 palette has 16 colors
- 1 color = RGB555 (16bit / little endian)

Bit-layout:

(low)

| Bit-7 | Bit-6 | Bit-5 | Bit-4 | Bit-3 | Bit-2 | Bit-1 | Bit-0 |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
|   -   |  r4   |  r3   |  r2   |  r1   |  r0   |  g4   |  g3   |

(high)

| Bit-7 | Bit-6 | Bit-5 | Bit-4 | Bit-3 | Bit-2 | Bit-1 | Bit-0 |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
|  g2   |  g1   |  g0   |  b4   |  b3   |  b2   |  b1   |  b0   |

- r4 ~ r0 : Red (0 ~ 31)
- g4 ~ g0 : Green (0 ~ 31)
- b4 ~ b0 : Blue (0 ~ 31)

#### Register #0: Scanline vertical counter (read only)

- $00 ~ $07 : Top blanking (0 ~ 7)
- $08 ~ $C7 : Active scanline (8 ~ 199)
- $FF : Bottom blanking (200 ~ 261)

#### Register #1: Scanline horizontal counter (read only)

- $00 ~ $07 : Left blanking
- $08 ~ $F7 : Active (rendering pixel position: 8 ~ 247)
- $F8 ~ $FF : Right blanking

#### Register #7: Status (read only)

| Bit-7 | Bit-6 | Bit-5 | Bit-4 | Bit-3 | Bit-2 | Bit-1 | Bit-0 |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
|  BL   |   -   |   -   |   -   |   -   |   -   |   -   |   -   |

- BL: 1 = start vblank

NOTE: Status register always reset after read.

(How to detect VBlank)

```z80
.wait_vblank
    ld hl, $9607
wait_vblank_loop:
    ld a, (hl)
    and $80
    jp nz, wait_vblank_loop
    ret
```

#### Character Pattern Table

- Character Pattern Table contains 256 pieces of 8x8 character data.
- One character is 32 bytes.
- Size of table: 32 x 256 = 8192 bytes

Bit layout:

| px0 | px1 | px2 | px3 | px4 | px5 | px6 | px7 | Line number |
| :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :---------- |
| H00 | L00 | H01 | L01 | H02 | L02 | H03 | L03 | Line 0      |
| H04 | L04 | H05 | L05 | H06 | L06 | H07 | L07 | Line 1      |
| H08 | L08 | H09 | L09 | H10 | L10 | H11 | L11 | Line 2      |
| H12 | L12 | H13 | L13 | H14 | L14 | H15 | L15 | Line 3      |
| H16 | L16 | H17 | L17 | H18 | L18 | H19 | L19 | Line 4      |
| H20 | L20 | H21 | L21 | H22 | L22 | H23 | L23 | Line 5      |
| H24 | L24 | H25 | L25 | H26 | L26 | H27 | L27 | Line 6      |
| H28 | L28 | H29 | L29 | H30 | L30 | H31 | L31 | Line 7      |

- `Hxx` : High 4bit (0 ~ 15 = Color Code) of byte index at xx (0 ~ 31)
- `Lxx` : Low 4bit (0 ~ 15 = Color Code) of byte index at xx (0 ~ 31)
- Color code 0 is transparent in FG and/or Sprite (not transparent in BG)

### 2-3. SCC

Waveform memory sound sources compatible with KONAMI's SCC can be used in FCS80.

| Address | r | w | Desciption |
|:-----------:|:-:|:-:|:-|
| $9800~$981F | o | o | 32 bytes signed to define the envelope (waveform) of the channel 1 |
| $9820~$983F | o | o | 32 bytes signed to define the envelope (waveform) of the channel 2 |
| $9840~$985F | o | o | 32 bytes signed to define the envelope (waveform) of the channel 3 |
| $9860~$987F | o | o | 32 bytes signed to define the envelope (waveform) of the channel 4 and 5 |
| $9880~$9881 | - | o | Channel 1 frequency on 12bit |
| $9882~$9883 | - | o | Channel 2 frequency on 12bit |
| $9884~$9885 | - | o | Channel 3 frequency on 12bit |
| $9886~$9887 | - | o | Channel 4 frequency on 12bit |
| $9888~$9889 | - | o | Channel 5 frequency on 12bit |
|    $988A    | - | o | Channel 1 volume (bits 4~7 are ignored) |
|    $988B    | - | o | Channel 2 volume (bits 4~7 are ignored) |
|    $988C    | - | o | Channel 3 volume (bits 4~7 are ignored) |
|    $988D    | - | o | Channel 4 volume (bits 4~7 are ignored) |
|    $988E    | - | o | Channel 5 volume (bits 4~7 are ignored) |
|    $988F    | - | o | ON/OFF switch for each channel from 1 to 5 (bits 0~4 = Channels 1~5) |
| $9890~$989F | - | o | Same as $9880~$988F (mirrors) |
| $98A0~$98DF | o | - | Channel 5 envelope data |
| $98E0~$98FF | - | - | Reserved (for Deformation register) |

NOTES:

- Frequencies ($9880~$9889) are the same format as for the [PSG frequency (registers 0~5)](#register-05-tone-generator)
- The address map is the same as most of KONAMI's MSX game software (*except Snatcher).

### 2-4. RAM

|  CPU address  |  RAM address  | Map                  |
| :-----------: | :-----------: | :------------------- |
| $C000 ~ $FFFF | $0000 ~ $3FFF | Random Access Memory |

## 3. I/O map

|   Port    |  I  |  O  | Description                                                   |
| :-------: | :-: | :-: | :------------------------------------------------------------ |
|    $A0    |  o  |  -  | AY-3-8910: latch register number                              |
|    $A1    |  -  |  o  | AY-3-8910: write register                                     |
|    $A2    |  -  |  o  | AY-3-8910: read register                                      |
|    $B0    |  o  |  o  | PRG0 bank switch (default: $00)                               |
|    $B1    |  o  |  o  | PRG1 bank switch (default: $01)                               |
|    $B2    |  o  |  o  | PRG2 bank switch (default: $02)                               |
|    $B3    |  o  |  o  | PRG3 bank switch (default: $03)                               |
|    $C0    |  -  |  o  | High Speed DMA (ROM to VRAM: Character Pattern Table)         |
|    $C1    |  o  |  o  | CPU boost flag                                                |
|    $C2    |  -  |  o  | High Speed DMA (memset)                                       |
|    $C3    |  -  |  o  | High Speed DMA (memcpy)                                       |
| $D0 ~ $DF |  o  |  o  | Direct read / write AY-3-8910 registers: #0 ($D0) ~ #15 ($DF) |

### 3-1. $A0~$A2 or $D0~$DF: AY-3-8910

| Register |  I  |  O  | Feature                        |
| :------: | :-: | :-: | :----------------------------- |
|    0     |  o  |  o  | Ch0 tone generator (low 8bit)  |
|    1     |  o  |  o  | Ch0 tone generator (high 4bit) |
|    2     |  o  |  o  | Ch1 tone generator (low 8bit)  |
|    3     |  o  |  o  | Ch1 tone generator (high 4bit) |
|    4     |  o  |  o  | Ch2 tone generator (low 8bit)  |
|    5     |  o  |  o  | Ch2 tone generator (high 4bit) |
|    6     |  o  |  o  | Noise generator                |
|    7     |  o  |  o  | Mixing                         |
|    8     |  o  |  o  | Ch0 volume                     |
|    9     |  o  |  o  | Ch1 volume                     |
|    10    |  o  |  o  | Ch2 volume                     |
|    11    |  o  |  o  | Envelope period (low 8bit)     |
|    12    |  o  |  o  | Envelope period (high 8bit)    |
|    13    |  o  |  o  | Envelope pattern               |
|    14    |  o  |  -  | JoyPad 1                       |
|    15    |  o  |  -  | JoyPad 2                       |

The FCS80 supports 16-ports access to simplify the creation of new programs, while also supports 3-ports access for simplify the porting of programs with using legacy AY-3-8910 register access.

- $A0~$A2: General AY-3-8910 register access port
- $D0~$DF: Direct AY-3-8910 register access port

#### Register 0~5: Tone generator

| Tone | Oct-1 | Oct-2 | Oct-3 | Oct-4 | Oct-5 | Oct-6 | Oct-7 | Oct-8 |
| :--: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
|  C   | $D5D  | $6AF  | $357  | $1AC  |  $D6  |  $6B  |  $35  |  $1B  |
|  C#  | $C9C  | $64E  | $327  | $194  |  $CA  |  $65  |  $32  |  $19  |
|  D   | $BE7  | $5F4  | $2FA  | $17D  |  $BE  |  $5F  |  $30  |  $18  |
|  D#  | $B3C  | $59E  | $2CF  | $168  |  $B4  |  $5A  |  $2D  |  $16  |
|  E   | $A9B  | $54E  | $2A7  | $153  |  $AA  |  $55  |  $2A  |  $15  |
|  F   | $A02  | $501  | $281  | $140  |  $A0  |  $50  |  $28  |  $14  |
|  F#  | $973  | $4BA  | $25D  | $12E  |  $97  |  $4C  |  $26  |  $13  |
|  G   | $8EB  | $476  | $23B  | $11D  |  $8F  |  $47  |  $24  |  $12  |
|  G#  | $86B  | $436  | $21B  | $10D  |  $87  |  $43  |  $22  |  $11  |
|  A   | $7F2  | $3F9  | $1FD  |  $FE  |  $7F  |  $40  |  $20  |  $10  |
|  A#  | $780  | $3C0  | $1E0  |  $F0  |  $78  |  $3C  |  $1E  |  $0F  |
|  B   | $714  | $38A  | $1C5  |  $E3  |  $71  |  $39  |  $1C  |  $0E  |

#### Register 6: Noise generator

| Bit-7 | Bit-6 | Bit-5 | Bit-4 | Bit-3 | Bit-2 | Bit-1 | Bit-0 |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
|   -   |   -   |   -   |  n4   |  n3   |  n2   |  n1   |  n0   |

#### Register 7: Mixing

| Bit-7 | Bit-6 | Bit-5 | Bit-4 | Bit-3 | Bit-2 | Bit-1 | Bit-0 |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
|   1   |   0   |  NE2  |  NE1  |  NE0  |  TE2  |  TE1  |  TE0  |

- NE2: Ch2 noise enable (1: enable, 0: disable)
- NE1: Ch1 noise enable (1: enable, 0: disable)
- NE0: Ch0 noise enable (1: enable, 0: disable)
- TE2: Ch2 tone enable (1: enable, 0: disable)
- TE1: Ch1 tone enable (1: enable, 0: disable)
- TE0: Ch0 tone enable (1: enable, 0: disable)

#### Register 8~10: Volume

(TODO: need describe)

#### Reigster 11~13: Envelope

(TODO: need describe)

#### Register 14~15: JoyPad

(TODO: need describe)

### 3-2. $B0~$B3: Bank switch

(TODO: need describe)

### 3-3. $C0: High Speed DMA (Bank to VRAM)

The ROM of the bank number corresponding to the value written to the port is transferred to the Character Pattern Table of VRAM.

#### ex) transfer Bank No.3 to the Character Pattern Table

```z80
    ld a, $03
    out ($C0), a
```

### 3-4. $C1: CPU boost flag

Normally, [consumeClock](src/z80.hpp#L340-L346) is called back at an interval of 4Hz or 3Hz (fixed at 4Hz in the case of LR35902), but by always fixing it to 1Hz during cpuBoostFlag is not zero, so the CPU execution speed can be accelerated by 4 to 3 times.

### 3-5. $C2: memset

It is a DMA equivalent to `memset` in C language:

- Register A: set value
- Register BC: destination address
- Register HL: transfer count in byte

### 3-6. $C3: memcpy

It is a DMA equivalent to `memcpy` in C language:

- Register A: ignored
- Register BC: destination address
- Register DE: source address
- Register HL: transfer count in byte

## 4. ROM

### 4-1. File Format

| Bank No. | Size | Content                        |
| :------: | :--: | :----------------------------- |
|    0     | 8KB  | Program (initial PRG0)         |
|    1     | 8KB  | Program or Data (initial PRG1) |
|    2     | 8KB  | Program or Data (initial PRG2) |
|    3     | 8KB  | Program or Data (initial PRG3) |
|    :     |  :   | :                              |
|   255    | 8KB  | Program, Data (switchable)     |

- The FCS80 ROM file is a set of 8 KB-delimited banks.
- The first bank (Bank No. 0) must contain the program.
- The second of later banks (Bank No. 1 to 255) contain the program or data.
- The size of the ROM file ranges from a minimum of 8 KB (1 bank) to 2 MB (256 banks)

## 5. Programming Guide

This chapter will guide you on how to implement a game that runs on the FCS80.

### 5-1. Make your development environment

- Z80 assembler (recommended: [z88dk](https://z88dk.org/site/))
- Graphic editor (recommended: [Asprite](https://store.steampowered.com/app/431730/Aseprite))

### 5-2. Example

| Name | Language | Description |
| :--- | :------- | :---------- | 
| Hello, World! | [Z80](./example/hello), [SDCC](./example/hello-sdcc) | Display "Hello, World!" in the BG, and scroll it by the input of JoyPad. |
| Sprite Test | [Z80](./example/sprite) | Render and move 256 sprites |
| Map Scroll | [Z80](./example/map_scroll), [SDCC](./example//map_scroll-sdcc/) | View and scroll through map data created with the [Tiled Map Editor](https://www.mapeditor.org). |
| Raster Scroll | [Z80](./example/raster_scroll) | Shake the screen by rewriting the scroll X for each scanline. |
| PSG Test (normal port)| [Z80](./example/psg) | Play 8 bars of Twinkle Twinkle Little Star on PSG with accompaniment. (using PSG port $A0 ~ $A2) |
| PSG Test (direct port)| [Z80](./example/psg_direct) | Play 8 bars of Twinkle Twinkle Little Star on PSG with accompaniment. (using PSG port $Dx) |

### 5-3. Notes for keeping the compatible

- Do not access to the Reserved area of [VRAM](#2-2-vram)
- Do not access to the undocumented [I/O](#3-io-map)
- Do not use the Refresh Register (`LD A, R` instruction)
  - RefreshRegister (RR) has different values depending on the type of Z80 CPU. Compatibility will be maintained as long as you continue to use the official emulator, but there is a possibility that compatibility will not be maintained when the actual device or an unofficial emulator appears in the future.

### 5-4. Toolchain

This repository provides a CLI toolchain that is useful for developing games that run on the FCS80.

| Name                           | Description                                                                                   |
| :----------------------------- | :-------------------------------------------------------------------------------------------- |
| [bmp2chr](tools/src/bmp2chr.c) | Converts a 128x128 pixel 8-bit color Bitmap to a Character Pattern Table format binary file.  |
| [csv2bin](tools/src/csv2bin.c) | Converts a CSV file output by [Tiled Map Editor](https://www.mapeditor.org) to a binary file. |
| [makerom](tools/src/makerom.c) | Concatenate binary files into a single file (ROM file) with mod 8KB                           |

## 6. HAL Implementation Guide

This chapter guides you through the implementation of a program to run the FCS80 emulator.
The FCS80 emulator provided in this repository can be compiled on any platform with GNU Compiler Collection or LLVM using the clang compiler. (ex: Windows, macOS, Linux, Android, iOS, Nintendo Switch, Sony PlayStation 3/4/5, Microsoft XBOX etc.)

### 6-1. Integrate it into your project

Please incorporate the [modules under the src directory of this repository](./src) into your project.

> Since all FCS80 emulator modules are provided in C++ header files (hpp), there is no need to add the modules to be compiled to the project.
> In other words, you can incorporate them by simply adding a reference to the include path (-I option in clang++ or include_directories in cmake) as a minimum support method.

### 6-2. How to use the FCS80 emulator

```c++
#include "fcs80.hpp"

{
    // make instance of FCS80 emulator
    FCS80 fcs80;

    // load ROM image
    fcs80.loadRomFile("path-of-rom-file");

    // tick 1 frame
    unsigned char pad1 = 0;
    unsigned char pad2 = 0;
    pad1 |= FCS80_JOYPAD_UP | FCS80_JOYPAD_T1; // ex: pushing UP and A button of 1P side
    fcs80.tick(pad1, pad2);

    // get display buffer (240x192 of RGB555)
    unsigned short* display = fcs80.getDisplay();

    // dequeue sound buffer (44100Hz 2ch 16bit PCM data)
    size_t sizeBytesOfSoundData;
    void* sound = fcs80.dequeuSoundBuffer(&sizeBytesOfSoundData);

    // save state
    size_t stateSize = fcs80.getStateSize();
    void* stateBuffer = malloc(stateSize);
    fcs80.saveState(stateBuffer);

    // load state
    fcs80.loadState(stateBuffer);

    // reset
    fcs80.reset();
}
```

### 6-3. Example

| Name | Platform | Description |
| :- | :-: | :- |
| [FCS80 for SDL2](./hal/sdl2) | macOS & Linux | FCS80 for SDL2 (Support macOS and Linux both) |
| [FCS80 for RPI0](./hal/rpizero/) | RaspberryPi Zero | Bare metal environment of RaspberryPi Zero |
| [FCS80 for RPI0-2W](./hal/rpizero2/) | RaspberryPi Zero 2W | Bare metal environment of RaspberryPi Zero 2W |
| [FCS80 for RPI1](./hal/rpizero/) | RaspberryPi | Bare metal environment of RaspberryPi _(Same as RPI0)_ |
| [FCS80 for RPI3](./hal/rpizero2/) | RaspberryPi 3 | Bare metal environment of RaspberryPi 3 _(Same as RPI0-2W)_ |
| [FCS80 for Cocoa](./hal/cocoa) | macOS | An example HAL implementation of FCS80 running on macOS. |
| [sndrec](./hal/sndrec) | Free | Starts the specified ROM file and recording the audio to a wav file. |
