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

hello.rom: hello.c fcs80.c fcs80.h crt0.s font.chr
	sdasz80 -o crt0.s
	sdcc -mz80 --code-loc 0x0100 --no-std-crt0 -Wlcrt0.rel -c fcs80.c
	sdcc -mz80 --code-loc 0x1000 --no-std-crt0 -Wlcrt0.rel -Wlfcs80.rel hello.c
	makebin -s 8192 hello.ihx hello.bin
	${MAKEROM} hello.rom hello.bin font.chr

font.chr: font.bmp
	${BMP2CHR} font.bmp font.chr
