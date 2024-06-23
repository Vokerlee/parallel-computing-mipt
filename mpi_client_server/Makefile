all:
	mkdir -p build
	mpicc client.c -O2 -lm -o ./build/client.out
	mpicc server.c -O2 -lm -o ./build/server.out
clean:
	rm -rf build

client:
	mpirun -np 1 ./build/client.out

server:
	mpirun -np 1 ./build/server.out
