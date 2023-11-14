CC   = gcc
OPTS = -O1 -lOpenCL
SRC  = main.c device.c
BIN  = intro.elf

all: build

build:
	$(CC) $(SRC) $(OPTS) -o $(BIN)

clean:
	rm -rf $(BIN)
