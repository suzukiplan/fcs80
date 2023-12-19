RASPPI=1
AARCH=32

all:
	cd src && git submodule update --init circle
	cd src/circle && ./makeall --nosample RASPPI=$(RASPPI) AARCH=$(AARCH)
	cd src/circle/addon/linux && make RASPPI=$(RASPPI) AARCH=$(AARCH)
	cd src/circle/addon/vc4 && ./makeall --nosample RASPPI=$(RASPPI) AARCH=$(AARCH)
	cd src && make -f Build.mk RASPPI=$(RASPPI) AARCH=$(AARCH)

clean:
	cd src && make -f Build.mk clean

cleanall:
	cd src/circle && ./makeall clean
	cd src/circle/addon/linux && make clean
	cd src/circle/addon/vc4 && ./makeall clean
	make clean
