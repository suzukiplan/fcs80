all:
	make execute-format FILENAME=./src/ay8910.hpp
	make execute-format FILENAME=./src/fcs80.hpp
	make execute-format FILENAME=./src/fcs80def.h
	make execute-format FILENAME=./src/fcs80video.hpp
	make execute-format FILENAME=./src/scc.hpp
	make execute-format FILENAME=./src/z80.hpp
	make execute-format FILENAME=./hal/sdl2/src/BufferQueue.cpp
	make execute-format FILENAME=./hal/sdl2/src/BufferQueue.h
	make execute-format FILENAME=./hal/sdl2/src/fcs80.cpp
	make execute-format FILENAME=./hal/sndrec/sndrec.cpp
	make execute-format FILENAME=./hal/rpizero/src/kernel.cpp
	make execute-format FILENAME=./hal/rpizero/src/kernel.h
	make execute-format FILENAME=./hal/rpizero/src/main.cpp
	make execute-format FILENAME=./hal/rpizero/src/std.c
	make execute-format FILENAME=./lib/sdcc/fcs80.c
	make execute-format FILENAME=./lib/sdcc/fcs80.h
	make execute-format FILENAME=./example/hello-sdcc/hello.c
	make execute-format FILENAME=./example/map_scroll-sdcc/program.c
	cd hal/sdl2 && make
	cd hal/rpizero && make
	cd hal/rpizero2 && make

execute-format:
	clang-format -style=file < ${FILENAME} > ${FILENAME}.bak
	cat ${FILENAME}.bak > ${FILENAME}
	rm ${FILENAME}.bak
