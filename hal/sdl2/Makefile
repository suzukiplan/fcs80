FCS80SRC = ../../src
CFLAGS = -O2
CFLAGS += -I./src
CFLAGS += -I$(FCS80SRC)
CFLAGS += -I/usr/include/SDL2
CFLAGS += -I/usr/local/include/SDL2
CFLAGS += -I/opt/X11/include
CFLAGS += -D_THREAD_SAFE
CC = gcc $(CFLAGS)

CPPFLAGS = -std=c++17
CPPFLAGS += -fno-rtti
CPPFLAGS += -fno-exceptions
CPPFLAGS += $(CFLAGS)
CPPFLAGS += -DZ80_DISABLE_DEBUG
CPPFLAGS += -DZ80_DISABLE_BREAKPOINT
CPPFLAGS += -DZ80_DISABLE_NESTCHECK
CPPFLAGS += -DZ80_CALLBACK_WITHOUT_CHECK
CPPFLAGS += -DZ80_CALLBACK_PER_INSTRUCTION
CPPFLAGS += -DZ80_UNSUPPORT_16BIT_PORT
CPPFLAGS += -DZ80_NO_FUNCTIONAL
CPPFLAGS += -DZ80_NO_EXCEPTION
CPP = g++ $(CPPFLAGS)

HEADER_FILES = $(FCS80SRC)/ay8910.hpp
HEADER_FILES += $(FCS80SRC)/fcs80.hpp
HEADER_FILES += $(FCS80SRC)/fcs80def.h
HEADER_FILES += $(FCS80SRC)/fcs80video.hpp
HEADER_FILES += $(FCS80SRC)/scc.hpp
HEADER_FILES += $(FCS80SRC)/z80.hpp
HEADER_FILES += $(FCS80SRC)/lz4.h
HEADER_FILES += $(FCS80SRC)/vgsdecv.hpp
HEADER_FILES += src/BufferQueue.h

OBJECTS = fcs80.o
OBJECTS += BufferQueue.o
OBJECTS += lz4.o
OBJECTS += vgstone.o

all: fcs80

clean:
	rm -f $(OBJECTS)
	rm -f fcs80

fcs80: $(OBJECTS)
	$(CPP) -o fcs80 $(OBJECTS) -L/usr/local/lib -lSDL2

BufferQueue.o: src/BufferQueue.cpp $(HEADER_FILES) ./Makefile
	$(CPP) -c $<

fcs80.o: src/fcs80.cpp $(HEADER_FILES) ./Makefile
	$(CPP) -c $<

lz4.o: ../../src/lz4.c $(HEADER_FILES) ./Makefile
	$(CC) -c $<

vgstone.o: ../../src/vgstone.c $(HEADER_FILES) ./Makefile
	$(CC) -c $<
