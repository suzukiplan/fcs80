all: bin bin/bmp2chr bin/makerom bin/csv2bin bin/makebgm
	cd vgsmml && make
	cp -p vgsmml/vgsmml bin

bin:
	mkdir bin

bin/bmp2chr: src/bmp2chr.c
	g++ -o bin/bmp2chr src/bmp2chr.c

bin/makerom: src/makerom.c
	g++ -o bin/makerom src/makerom.c

bin/csv2bin: src/csv2bin.c
	g++ -o bin/csv2bin src/csv2bin.c

bin/makebgm: src/makebgm.cpp
	g++ -o bin/makebgm src/makebgm.cpp
