all:server client

server:server.o
	gcc -g -o server server.o
client:client.o
	gcc -g -o client client.o

server.o:server.c
	gcc -g -c server.c
client.o:client.c
	gcc -g -c client.c

clean:
	rm server client *.o