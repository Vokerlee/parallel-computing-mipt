all:
	mkdir -p build
	mpicc main.c -O2 -lgmp -lm -o ./build/main.out

clean:
	rm -rf build

run:
	mpirun -np 2 ./build/main.out 100000000
