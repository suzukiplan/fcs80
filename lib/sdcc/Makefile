all:
	sdasz80 -o crt0.s
	sdcc -mz80 --opt-code-size --code-loc 0x0100 --no-std-crt0 -c fcs80.c
	sdar -rc fcs80.lib crt0.rel fcs80.rel

doc: fcs80.h
	doxygen -s Doxyfile

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
	rm -f *.lib
