# PSG test - direct edition

- Play 8 bars of Twinkle Twinkle Little Star on PSG with accompaniment.
- Using original PSG Sound Driver for Z80 ([psgdrv.asm](psgdrv.asm)) direct edition

## How to build

### Pre-request

- GNU make
- clang (for building the depended tools)
- [z88dk](https://z88dk.org/site/)
  - require `z80asm` command in this example

### Build

```zsh
cd fcs80/example/psg_direct
make
```

## PSG Sound Driver [psgdrv.asm](psgdrv.asm)

Same specifications as [psg](../psg/README.md), but there are the some differences in [implementation](https://github.com/suzukiplan/fcs80/pull/2/commits/ea1218d61b20edbaf16a1303bc5ee01d6bdbff0f)

## RAM usage

Same specifications as [psg](../psg/README.md)
