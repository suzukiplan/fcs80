# PSG test

- Play 8 bars of Twinkle Twinkle Little Star on PSG with accompaniment.
- Using original PSG Sound Driver for Z80 ([psgdrv.asm](psgdrv.asm))

## How to build

### Pre-request

- GNU make
- clang (for building the depended tools)
- [z88dk](https://z88dk.org/site/)
  - require `z80asm` command in this example

### Build

```zsh
cd fcs80/example/psg
make
```

## PSG Sound Driver [psgdrv.asm](psgdrv.asm)

### Sequence data specification

| Byte 1 | Byte 2 | Description                                         |
| :----: | :----: | :-------------------------------------------------- |
|  $0x   |  $xx   | Ch0 tone = $xxx                                     |
|  $1x   |  $xx   | Ch1 tone = $xxx                                     |
|  $2x   |  $xx   | Ch2 tone = $xxx                                     |
|  $3x   |   -    | Ch0 key-on & initial volume (0 ~ 15)                |
|  $4x   |   -    | Ch0 key-on & initial volume (0 ~ 15)                |
|  $5x   |   -    | Ch0 key-on & initial volume (0 ~ 15)                |
|  $60   |  $xx   | Ch0 volume down interval (software envelope of Ch0) |
|  $61   |  $xx   | Ch1 volume down interval (software envelope of Ch1) |
|  $62   |  $xx   | Ch2 volume down interval (software envelope of Ch2) |
|  $63   |  $xx   | Wait $xx frames (1 frame = 1/60 sec)                |
|  $64   |  $xx   | Noise = $xx                                         |
|  $65   |  $xx   | Mixing = $xx                                        |
|  $66   |   -    | Set loop mark                                       |
|  $67   |   -    | Jump to loop mark                                   |
|  $70   |  $xx   | Ch0 pitch down (0: disabled, 1~255: down Hz/f)      |
|  $71   |  $xx   | Ch1 pitch down (0: disabled, 1~255: down Hz/f)      |
|  $72   |  $xx   | Ch2 pitch down (0: disabled, 1~255: down Hz/f)      |
|  $73   |  $xx   | Ch0 pitch up (0: disabled, 1~255: up Hz/f)          |
|  $74   |  $xx   | Ch1 pitch up (0: disabled, 1~255: up Hz/f)          |
|  $75   |  $xx   | Ch2 pitch up (0: disabled, 1~255: up Hz/f)          |
|  $FF   |   -    | End of sequence data                                |

## RAM usage

| Offset | Usage                    |
| :----: | :----------------------- |
| $0000  | Sequence position (low)  |
| $0001  | Sequence position (high) |
| $0002  | Wait Counter             |
| $0003  | Ch0 volume down interval |
| $0004  | Ch1 volume down interval |
| $0005  | Ch2 volume down interval |
| $0006  | Ch0 volume down counter  |
| $0007  | Ch1 volume down counter  |
| $0008  | Ch2 volume down counter  |
