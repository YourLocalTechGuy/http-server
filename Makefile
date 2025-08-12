CC=gcc
CFLAGS=-Wall -Wextra -g -static
SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
TARGET=hserver

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)