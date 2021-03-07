BMP2CHR = ../../tools/bin/bmp2chr
MAKEROM = ../../tools/bin/makerom

all: ${BMP2CHR} ${MAKEROM} sprite.rom

clear:
	rm -f *.bin *.o *.chr

${BMP2CHR}:
	cd ../../tools && make

${MAKEROM}:
	cd ../../tools && make

sprite.rom: sprite.bin image.chr
	${MAKEROM} sprite.rom sprite.bin image.chr

sprite.bin: sprite.asm
	z80asm -b sprite.asm

image.chr: image.bmp
	${BMP2CHR} image.bmp image.chr
