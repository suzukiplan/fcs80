PRJ = psg
MAKEROM = ../../tools/bin/makerom

all: ${MAKEROM} ${PRJ}.rom

clear:
	rm -f *.bin *.o *.chr

${MAKEROM}:
	cd ../../tools && make

${PRJ}.rom: program.bin
	${MAKEROM} ${PRJ}.rom program.bin

program.bin: program.asm psgdrv.asm
	z80asm -b program.asm
