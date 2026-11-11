CC = gcc
CFLAGS = -Wall -Wextra -g

all: libmonitor.a server client

# Compilar funciones auxiliares y empaquetar biblioteca estática
libmonitor.a: bd.o utils.o
	ar rcs libmonitor.a bd.o utils.o

bd.o: bd.c monitor.h
	$(CC) $(CFLAGS) -c bd.c -o bd.o

utils.o: utils.c monitor.h
	$(CC) $(CFLAGS) -c utils.c -o utils.o

# Compilar servidor
server: server.c libmonitor.a
	$(CC) $(CFLAGS) server.c -L. -lmonitor -o server

# Compilar cliente
client: client.c
	$(CC) $(CFLAGS) client.c -o client

clean:
	rm -f *.o libmonitor.a server client

.PHONY: all clean
