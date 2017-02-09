CFLAGS=-lpthread -std=c++11

pa2.x : pa2.c
	g++ -o $@ $^ $(CFLAGS)

run: pa2.x
	./$^ 1 3 3 3 a b c
	./$^ 1 3 5 6 a b c
	./$^ 1 3 4 9 a b c
	./$^ 1 3 7 3 a b c

clean:
	rm pa2.x

    

