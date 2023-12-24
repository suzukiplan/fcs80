BMP2CHR = ../../tools/bin/bmp2chr
MAKEROM = ../../tools/bin/makerom
CSV2BIN = ../../tools/bin/csv2bin
VGSMML = ../../tools/bin/vgsmml

all:
	cd ../../tools && make
	make vgs.rom song.bgm
	cd ../../hal/sdl2 && make
	../../hal/sdl2/fcs80 -v song.bgm vgs.rom

clean:
	rm -f *.bgm
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

vgs.rom: program.bin font.chr
	${MAKEROM} vgs.rom program.bin font.chr

program.bin: program.c ../../lib/sdcc/fcs80.lib ../../lib/sdcc/fcs80.h
	cd ../../lib/sdcc && make
	sdcc -mz80 --opt-code-size --code-loc 0x100 --no-std-crt0 -Wl../../lib/sdcc/fcs80.lib program.c
	makebin -s 16384 program.ihx program.bin

font.chr: font.bmp
	${BMP2CHR} font.bmp font.chr

song.bgm: song.mml
	${VGSMML} $< $@
