BMP2CHR = ../../tools/bin/bmp2chr
MAKEROM = ../../tools/bin/makerom

all: ${BMP2CHR} ${MAKEROM} hello.rom

clear:
	rm -f *.bin *.o *.chr

${BMP2CHR}:
	cd ../../tools && make

${MAKEROM}:
	cd ../../tools && make

hello.rom: hello.bin font.chr
	${MAKEROM} hello.rom hello.bin font.chr

hello.bin: hello.asm
	z80asm -b hello.asm

font.chr: font.bmp
	${BMP2CHR} font.bmp font.chr
