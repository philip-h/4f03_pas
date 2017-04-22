pa5.x : pa5.cu ppmFile.cu
	nvcc -o $@ $^

run: pa5.x
	./$^ 3 "fox.ppm" "out.ppm"

clean:
	rm pa5.x out.ppm
