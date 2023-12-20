all:
	cp -p ../rpizero/src/Build.mk src
	cp -p ../rpizero/src/kernel.cpp src
	cp -p ../rpizero/src/kernel.h src
	cp -p ../rpizero/src/main.cpp src
	cp -p ../rpizero/src/std.c src
	cp -p ../rpizero/src/splash.c src
	cd src && git submodule update --init circle
	cp src/Config.mk src/circle
	cd src/circle && ./makeall --nosample
	cd src/circle/addon/fatfs && make
	cd src/circle/addon/SDCard && make
	cd src/circle/addon/linux && make
	cd src/circle/addon/vc4 && ./makeall --nosample
	cd src && make -f Build.mk
	cp src/kernel8.img image/kernel8.img

clean:
	cd src && make -f Build.mk clean

cleanall:
	cd src/circle && ./makeall clean
	cd src/circle/addon/fatfs && clean
	cd src/circle/addon/SDCard && make clean
	cd src/circle/addon/linux && make clean
	cd src/circle/addon/vc4 && ./makeall clean
	make clean
