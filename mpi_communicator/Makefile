all:
	mkdir -p build
	mpicc main.c -O2 -lm -o ./build/main.out

clean:
	rm -rf build

run:
	mpirun -np 4 ./build/main.out 1000
