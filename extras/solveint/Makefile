.PHONY: all run clean spotless

all: solveint

run: all
	./solveint

solveint: solveint.pas
	fpc -Miso -Co -CO $^

clean:
	rm -f *.o

spotless: clean
	rm -f solveint
