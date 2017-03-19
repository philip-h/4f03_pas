$FLAGS=-lnsl

all: client server_append server_verify 

client: client.c clnt.c
	gcc $^ -o $@ $(FLAGS)

server_append: append_server.c append_svc.c
	gcc $^ -o $@ $(FLAGS)

server_verify: verify_server.c verify_svc.c
	gcc $^ -o $@ $(FLAGS)

clean:
	$(RM) client server_append server_verify 
