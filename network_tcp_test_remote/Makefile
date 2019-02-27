all:server client tcp_client

server:server.o
	gcc -g -o server server.o
client:client.o
	gcc -g -o client client.o
tcp_client:tcp_client.o
	gcc -g -o tcp_client tcp_client.o
server.o:server.c
	gcc -g -c server.c
client.o:client.c
	gcc -g -c client.c
tcp_client.o:tcp_client.c
	gcc -g -c tcp_client.c
clean:
	rm server client tcp_client *.o