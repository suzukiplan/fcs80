PRJ = raster_scroll
BMP2CHR = ../../tools/bin/bmp2chr
MAKEROM = ../../tools/bin/makerom
CSV2BIN = ../../tools/bin/csv2bin

all: ${BMP2CHR} ${MAKEROM} ${CSV2BIN} ${PRJ}.rom

clear:
	rm -f *.bin *.o *.chr

${BMP2CHR}:
	cd ../../tools && make

${MAKEROM}:
	cd ../../tools && make

${CSV2BIN}:
	cd ../../tools && make

${PRJ}.rom: program.bin image.chr mapdata.bin
	${MAKEROM} ${PRJ}.rom program.bin image.chr mapdata.bin

program.bin: program.asm
	z80asm -b program.asm

image.chr: image.bmp
	${BMP2CHR} image.bmp image.chr

mapdata.bin: mapdata.csv
	${CSV2BIN} mapdata.csv mapdata.bin
