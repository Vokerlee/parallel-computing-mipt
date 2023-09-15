CC   = gcc
OPTS = -fopenmp
SRC  = main.c
BIN  = copy.elf

all: build_copyin_on

build_copyin_on:
	$(CC) $(SRC) $(OPTS) -DCOPYIN_ON -o $(BIN)

build_copyin_off:
	$(CC) $(SRC) $(OPTS) -o $(BIN)

clean:
	rm -rf $(BIN)
