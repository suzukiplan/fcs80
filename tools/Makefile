all: bin bin/bmp2chr bin/makerom

bin:
	mkdir bin

bin/bmp2chr: src/bmp2chr.c
	clang -o bin/bmp2chr src/bmp2chr.c

bin/makerom: src/makerom.c
	clang -o bin/makerom src/makerom.c
