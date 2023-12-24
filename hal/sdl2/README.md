# FCS80 for SDL2

## How to Build & Execute

### macOS

```bash
# NOTE: XCode CLI Tools required

# install SDL2
brew install sdl2

# download repository
git clone https://github.com/suzukiplan/fcs80

# move to this dir
cd fcs80/hal/sdl2

# build
make

# execute
./fcs80 ../../example/hello/hello.rom
```

### Linux

```bash
# install GNU Make, GCC and other
sudo apt install build-essential

# install SDL2
sudo apt-get install libsdl2-dev

# install ALSA
sudo apt-get install libasound2
sudo apt-get install libasound2-dev

# download repository
git clone https://github.com/suzukiplan/fcs80

# move to this dir
cd fcs80/hal/sdl2

# build
make

# execute
./fcs80 ../../example/hello/hello.rom
```

## Usage

```
usage: fcs80 /path/to/file.rom ....... Specify ROM file to be used
             [-v /path/to/bgm.vgs] ... VGS BGM data (max 256 data)
             [-g { None .............. GPU: Do not use
                 | OpenGL ............ GPU: OpenGL <default>
                 | Vulkan ............ GPU: Vulkan
                 | Metal ............. GPU: Metal
                 }]
             [-f] .................... Full Screen Mode
```