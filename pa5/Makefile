out.x : pa5.cu ppmFile.cu
	nvcc -o $@ $^

run: out.x
	./$^ 10 fox4k.ppm out.ppm

clean:
	$(RM) out.x out.ppm
