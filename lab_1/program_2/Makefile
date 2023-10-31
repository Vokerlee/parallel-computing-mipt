CC   = gcc
OPTS = -O2 -fopenmp -lm
SRC  = main.c
BIN  = task_2.elf

DEFS = ISIZE=5000 JSIZE=5000 N_THREADS=2
OPTS_DEFS = $(addprefix -D,$(DEFS))

all: build

build:
	$(CC) $(SRC) $(OPTS) $(OPTS_DEFS) -o $(BIN)

launch:
	./$(BIN)

debug:
	$(CC) $(SRC) $(OPTS) $(OPTS_DEFS) -DDEBUG_ON -o $(BIN)

clean:
	rm -rf *.elf
