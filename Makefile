#get macports
#install libsdl

CC=gcc
CFLAGS=-I/opt/local/include -framework cocoa -std=c99 -Wall -mmacosx-version-min=10.5
LDFLAGS=-lSDL -lSDLmain -L/opt/local/lib

all : pixie

pixie : pixie.c
	$(CC) pixie.c $(LDFLAGS) $(CFLAGS) -o pixie

run : pixie
	-./pixie

clean :
	-rm -f pixie
