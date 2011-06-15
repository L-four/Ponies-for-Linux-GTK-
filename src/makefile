CC=gcc
SOURCES=main.c ponies.c
EXECUTABLE=ponies

all:
	$(CC) `pkg-config --cflags --libs gtk+-2.0` -std=c99 $(SOURCES) -o $(EXECUTABLE)
