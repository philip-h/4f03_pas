CFLAGS=-lpthread -std=c++11

pa2.x : pa2.c
	g++ -o $@ $^ $(CFLAGS)

run: pa2.x
	./$^ 0 3 6 3 a b c

clean:
	rm pa2.x


