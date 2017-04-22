pa5.x : pa5.c ppmFile.c
	nvcc -o $@ $^

run: pa5.x
	./$^ 3 "fox.ppm" "out.ppm"


clean:
	rm pa5.x out.ppm
