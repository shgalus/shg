# Makefile: description file for make in shg/examples directory

include ../make.inc

TARGET = comblex matrixc matrixo matrixi matrixm wolfe

all: $(TARGET)

clean:
	rm -f $(TARGET) $(TARGET:%=%.d)
spotless: clean
