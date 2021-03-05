# [WIP] FAIRY COMPUTER SYSTEM 80

## Specification

- CPU: Z80A (3579545Hz)
- PPU: FCS80-VDP (4024320Hz)
  - Screen Resolution: 240 x 192
  - Background & Foreground 8x8 tiled graphics (both virtual screen: 256 x 256)
  - 256 sprites (size: 8x8) & max horizontal limit: 256 (unlimited)
  - Number of Scanlines: 262
  - Frame Rate: 60 frames per second
  - Colors: 256 colors in 65536 colors
- APU: AY-3-8910 (44100Hz)
- RAM: 16KB
- VRAM: 16KB

## Memory Map

### CPU Memory

|     Address     | Map                           |
| :-------------: | :---------------------------- |
| 0x0000 ~ 0x1FFF | ROM Bank #0 (Program)         |
| 0x2000 ~ 0x3FFF | ROM Bank #1 (Program or Data) |
| 0x4000 ~ 0x5FFF | ROM Bank #2 (Program or Data) |
| 0x6000 ~ 0x7FFF | ROM Bank #3 (Program or Data) |
| 0x8000 ~ 0xBFFF | VRAM (16KB)                   |
| 0xC000 ~ 0xFFFF | RAM (16KB)                    |

### VRAM

|   CPU address   |  VRAM address   | Map                                                     |
| :-------------: | :-------------: | :------------------------------------------------------ |
| 0x8000 ~ 0x83FF | 0x0000 ~ 0x03FF | BG Name Table (32 x 32)                                 |
| 0x8400 ~ 0x87FF | 0x0400 ~ 0x07FF | BG Attribute Table (32 x 32)                            |
| 0x8800 ~ 0x8BFF | 0x0800 ~ 0x0BFF | FG Name Table (32 x 32)                                 |
| 0x8C00 ~ 0x8FFF | 0x0C00 ~ 0x0FFF | FG Attribute Table (32 x 32)                            |
| 0x9000 ~ 0x93FF | 0x1000 ~ 0x13FF | OAM; Object Attribute Memory (4 x 256)                  |
| 0x9400 ~ 0x95FF | 0x1400 ~ 0x15FF | Palette Table (2 x 16 x 16)                             |
|     0x9600      |     0x1600      | Register #0: Scanline vertical counter (read only)      |
|     0x9601      |     0x1601      | Register #1: Scanline horizontal counter (read only)    |
|     0x9602      |     0x1602      | Register #2: BG Scroll X                                |
|     0x9603      |     0x1603      | Register #3: BG Scroll Y                                |
|     0x9604      |     0x1604      | Register #4: FG Scroll X                                |
|     0x9605      |     0x1605      | Register #5: FG Scroll Y                                |
|     0x9606      |     0x1606      | Register #6: IRQ scanline position (NOTE: 0 is disable) |
| 0x9607 ~ 0x9FFF | 0x1607 ~ 0x1FFF | Reserved (cannot write and always return 0xFF)          |
| 0xA000 ~ 0xBFFF | 0x2000 ~ 0x3FFF | Character Pattern Table (32 x 256)                      |

#### Writing priorities

1. FG
2. Sprite (NOTE: the sprite with the highest OAM index value will be displayed first)
3. BG

#### Name Table & Scroll (BG/FG)

- 32 x 32 characters = 256 x 256 pixels
- masked the edge 8px of left, right and top
- masked the bottom edge 56px
- scrollable with Scroll Register (#2 ~ E5)

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

#### Register #0: Scanline vertical counter (read only)

- 0x00 ~ 0x07 : Top blanking (0 ~ 7)
- 0x08 ~ 0xC7 : Active scanline (8 ~ 199)
- 0xFF : Bottom blanking (200 ~ 261)

(How to detect VBlank)

```z80
.wait_vblank
    ld hl, 0x9600
wait_vblank_loop:
    ld a, (hl)
    cp 0xff
    jp nz, wait_vblank_loop
    ret
```

#### Register #1: Scanline horizontal counter (read only)

- 0x00 ~ 0x07 : Left blanking
- 0x08 ~ 0xF7 : Active (rendering pixel position: 8 ~ 247)
- 0xF8 ~ 0xFF : Right blanking

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

- `Hxx` : High 4bit of byte index at xx
- `Lxx` : Low 4bit of byte index at xx
- `xx` : Color code (0 ~ 15)
- Color code 0 is transparent in FG and/or Sprite (not transparent in BG)

## I/O map

| Port |  I  |  O  | Description                      |
| :--: | :-: | :-: | :------------------------------- |
| 0xA0 |  o  |  -  | AY-3-8910: latch register number |
| 0xA1 |  -  |  o  | AY-3-8910: write register        |
| 0xA2 |  -  |  o  | AY-3-8910: read register         |
| 0xB0 |  o  |  o  | PRG0 bank switch (default: 0x00) |
| 0xB1 |  o  |  o  | PRG1 bank switch (default: 0x01) |
| 0xB2 |  o  |  o  | PRG2 bank switch (default: 0x02) |
| 0xB3 |  o  |  o  | PRG3 bank switch (default: 0x03) |
| 0xC0 |  -  |  o  | High Speed DMA (memmove)         |
| 0xC1 |  -  |  o  | High Speed DMA (memset)          |

### 0xC0: High Speed DMA (memmove)

- Transfer destination address: Register BC
- Transfer size: Register DE
- Transfer source address: Value written to port 0xC0 x 0x100

The following is the source code for transferring the contents of PRG3 (0x6000 ~ 0x7FFF) to the Character Pattern Table (0xA000 ~ 0xBFFF).

```z80
    ld bc, 0xA000
    ld de, 0x2000
    ld a, 0x60
    out (0xC0), a
```

### 0xC1: High Speed DMA (memset)

- Transfer destination address: Register BC
- Transfer size: Register DE
- Set value written to port 0xC1

The following is the source code for reset Character Pattern Table (0xA000 ~ 0xBFFF).

```z80
    ld bc, 0xA000
    ld de, 0x2000
    ld a, 0x00
    out (0xC1), a
```

## ROM

### File Format

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
