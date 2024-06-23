all:
	mkdir -p build
	mpicc main.c -O2 -lm -o ./build/sorting.out

clean:
	rm -rf build

run:
	mpirun -np 4 ./build/sorting.out 10
