CFLAGS= -std=c99

pa4.x : pa4.c ppmFile.c
	mpicc -o $@ $^ $(CFLAGS)

run: pa4.x
	/usr/lib64/openmpi/bin/mpirun -np 10 ./$^ 5 "small.ppm" "out.ppm"


clean:
	rm pa4.x out.ppm
