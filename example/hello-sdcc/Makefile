BMP2CHR = ../../tools/bin/bmp2chr
MAKEROM = ../../tools/bin/makerom

all: ${BMP2CHR} ${MAKEROM} hello.rom
	cd ../../hal/sdl2 && make
	../../hal/sdl2/fcs80 hello.rom

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

hello.rom: hello.bin font.chr
	${MAKEROM} hello.rom hello.bin font.chr

hello.bin: hello.c ../../lib/sdcc/fcs80.lib ../../lib/sdcc/fcs80.h
	cd ../../lib/sdcc && make
	sdcc -mz80 --opt-code-size --code-loc 0x100 --no-std-crt0 -Wl../../lib/sdcc/fcs80.lib hello.c
	makebin -s 16384 hello.ihx hello.bin

font.chr: font.bmp
	${BMP2CHR} font.bmp font.chr
