BMP2CHR = ../../tools/bin/bmp2chr
MAKEROM = ../../tools/bin/makerom
CSV2BIN = ../../tools/bin/csv2bin
CFLAGS = 

all: ${BMP2CHR} ${MAKEROM} ${CSV2BIN} map_scroll.rom
	cd ../../hal/sdl2 && make
	../../hal/sdl2/fcs80 map_scroll.rom

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

${CSV2BIN}:
	cd ../../tools && make

map_scroll.rom: program.bin image.chr mapdata.bin
	${MAKEROM} map_scroll.rom program.bin image.chr mapdata.bin

program.bin: program.c
	cd ../../lib/sdcc && make
	sdcc -mz80 --opt-code-size --code-loc 0x100 --no-std-crt0 -Wl../../lib/sdcc/fcs80.lib program.c
	makebin -s 16384 program.ihx program.bin

image.chr: image.bmp
	${BMP2CHR} image.bmp image.chr

mapdata.bin: mapdata.csv
	${CSV2BIN} mapdata.csv mapdata.bin

