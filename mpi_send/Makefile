all:
	mkdir -p build
	mpicc main.c -DSEND -O2 -lm -o ./build/main.out

clean:
	rm -rf build

send:
	mpicc main.c -DSEND -O2 -lm -o ./build/main.out
	mpirun -np 2 ./build/main.out 10
	mpirun -np 2 ./build/main.out 1000
	mpirun -np 2 ./build/main.out 100000

rsend:
	mpicc main.c -DRSEND -O2 -lm -o ./build/main.out
	mpirun -np 2 ./build/main.out 10
	mpirun -np 2 ./build/main.out 1000
	mpirun -np 2 ./build/main.out 100000

ssend:
	mpicc main.c -DSSEND -O2 -lm -o ./build/main.out
	mpirun -np 2 ./build/main.out 10
	mpirun -np 2 ./build/main.out 1000
	mpirun -np 2 ./build/main.out 100000

bsend:
	mpicc main.c -DBSEND -O2 -lm -o ./build/main.out
	mpirun -np 2 ./build/main.out 10
	mpirun -np 2 ./build/main.out 1000
	mpirun -np 2 ./build/main.out 100000

