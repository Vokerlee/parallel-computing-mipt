CC   = mpicc
OPTS = -O2 -lm
SRC  = main.c
BIN  = task_3.elf

DEFS = ISIZE=5000 JSIZE=5000
N_THREADS = 2

OPTS_DEFS = $(addprefix -D,$(DEFS))

all: build

build:
	$(CC) $(SRC) $(OPTS) $(OPTS_DEFS) -o $(BIN)

launch:
	mpirun -np $(N_THREADS) ./$(BIN)

debug:
	$(CC) $(SRC) $(OPTS) -DDEBUG_ON $(OPTS_DEFS) -o $(BIN)

clean:
	rm -rf *.elf
