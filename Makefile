SERVER_SRCS += server.c
SERVER_SRCS += queue.c

all: server client
server: $(SERVER_SRCS)
	gcc $(SERVER_SRCS) -I. -lpthread -o server
client: client.c
	gcc client.c -lpthread -o client
clean:
	rm server client
