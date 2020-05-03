# Makefile: description file for make in shg/examples directory

include ../make.inc

TARGET = matrixc matrixo matrixi matrixm wolfe

all: $(TARGET)

clean:
	rm -f $(TARGET) $(TARGET:%=%.d)
spotless: clean
