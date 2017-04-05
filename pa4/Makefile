CFLAGS= -std=c99

pa4.x : pa4.c ppmFile.c
	gcc -o $@ $^ $(CFLAGS)

run: pa4.x
	./$^


clean:
	rm pa4.x out.ppm
