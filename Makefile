CFLAGS=-lpthread -std=c++11

pa2.x : pa2.c
	g++ -o $@ $^ $(CFLAGS)

run: pa2.x
	./$^ 1 4 5 4 a b c
stop: pa2.x
	./$^ 1 3 2 3 a b c
	./$^ 1 3 4 3 a b c
	./$^ 1 3 3 3 a b c
	./$^ 1 3 5 6 a b c
	./$^ 1 3 7 3 a b c


clean:
	rm pa2.x

    

