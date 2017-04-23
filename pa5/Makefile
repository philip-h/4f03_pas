out.x : pa5.cu ppmFile.cu
	nvcc -o $@ $^

run: out.x
	./$^ 3 fox2.ppm out.ppm

clean:
	$(RM) out.x out.ppm
