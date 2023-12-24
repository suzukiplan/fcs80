BMP2CHR = ../../tools/bin/bmp2chr
MAKEROM = ../../tools/bin/makerom
CFLAGS = 

all: ${BMP2CHR} ${MAKEROM} sprite.rom
	cd ../../hal/sdl2 && make
	../../hal/sdl2/fcs80 sprite.rom

clean:
	rm -f *.rel
	rm -f *.chr
	rm -f *.lst
	rm -f *.sym
	rm -f *.asm
	rm -f *.ihx
	rm -f *.map
	rm -f *.bin
	rm -f *.lk
	rm -f *.noi
	rm -f *.rom

${BMP2CHR}:
	cd ../../tools && make

${MAKEROM}:
	cd ../../tools && make

sprite.rom: program.bin image.chr
	${MAKEROM} sprite.rom program.bin image.chr

program.bin: program.c ../../lib/sdcc/fcs80.lib ../../lib/sdcc/fcs80.h
	cd ../../lib/sdcc && make
	sdcc -mz80 --opt-code-size --code-loc 0x100 --xram-loc 0xC000 --no-std-crt0 -Wl../../lib/sdcc/fcs80.lib program.c
	makebin -s 16384 program.ihx program.bin

image.chr: image.bmp
	${BMP2CHR} image.bmp image.chr
