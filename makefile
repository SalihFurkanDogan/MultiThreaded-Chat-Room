CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -pthread
SRC = src/Client.c \
      src/Parse_Command.c \
      src/Room.c \
      src/Server.c \
      src/User.c \
      server_main.c
OBJ = $(SRC:.c=.o)
OUT = server

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OUT) *.o src/*.o

.PHONY: all clean
