CFLAGS=-lpthread -std=c++11

pa2.x : pa2.c
	g++ -o $@ $^ $(CFLAGS)

run: pa2.x
	./$^ 2 3 1 3 a b c


clean:
	rm pa2.x

    

