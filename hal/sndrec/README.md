# Sound Recorder (CLI)

- Starts the specified ROM file and recording the audio to a wav file.
- This HAL implementation runs platform-free

## Usage

```sh
sndrec input_rom_file output_wave_file play_frames
```

> Silence data (prefix) and one second of fade-out data (suffix) are automatically appended.

## How to build

### Pre-request

- GNU make
- clang++

### Build

```zsh
cd fcs80/hal/sndrec
make
```
