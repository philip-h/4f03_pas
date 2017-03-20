$FLAGS=-lnsl


all: client server_append server_verify

run : client
	./client 0 3 4 3 a b c localhost localhost

client: client.c clnt.c verify_xdr.c append_xdr.c
	gcc $^ -o $@ $(FLAGS) -fopenmp

server_append: append_server.c append_svc.c append_xdr.c
	gcc $^ -o $@ $(FLAGS)

server_verify: verify_server.c verify_svc.c verify_xdr.c
	gcc $^ -o $@ $(FLAGS) -lpthread

clean:
	$(RM) client server_append server_verify
