MPI_CC    = mpicc
OPENMP_CC = gcc
OPTS      = -O2 -lm

MPI_SRC    = main_mpi.c
OPENMP_SRC = main_openmp.c

MPI_BIN     = ref_mpi.elf
OPENMP_BIN = ref_openmp.elf

DEFS = ISIZE=5000 JSIZE=5000
N_THREADS = 2

OPTS_DEFS = $(addprefix -D,$(DEFS))

all: build

build:
	$(MPI_CC)    $(MPI_SRC)    $(OPTS) $(OPTS_DEFS) -o $(MPI_BIN)
	$(OPENMP_CC) $(OPENMP_SRC) $(OPTS) $(OPTS_DEFS) -fopenmp -DN_THREADS=$(N_THREADS) -o $(OPENMP_BIN)

mpi_launch:
	mpirun -np $(N_THREADS) ./$(MPI_BIN)

openmp_launch:
	./$(OPENMP_BIN)

clean:
	rm -rf *.elf
