
OBJS_SERVER = bcast_server.o
OBJS_CLIENT = bcast_client.o
LIBS_SERVER = 
LIBS_CLIENT = 

CFLAGS = -c
CC = gcc

PROS = bcast_client bcast_server

all: $(PROS)

.c.o:
	$(CC) $(CFLAGS) $<

bcast_client: $(OBJS_CLIENT)
	$(CC) -o $@ $^ $(LIBS_SERVER)

bcast_server: $(OBJS_SERVER)
	$(CC) -o $@ $^ $(LIBS_CLIENT)

clean:
	rm -rf $(PROS) $(OBJS_CLIENT) $(OBJS_SERVER)