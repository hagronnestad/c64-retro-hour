# do not remove intermediate targets
.SECONDARY:

name := c64retrohour

.PHONY: build
all:
	mkdir -p build
	cl65 -Ois main.c -o build/$(name).prg
	rm *.o

.PHONY: clean
clean:
	rm -rf build
