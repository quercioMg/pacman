CC = gcc
CFLAGS = -Wall -std=c99 -Isrc
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Tutti i file sono dentro src
SRCS = src/main.c src/entities.c src/map.c src/ghost.c
TARGET = pacman

all:
	$(CC) $(SRCS) -o $(TARGET) $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)
