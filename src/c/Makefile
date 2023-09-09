CFLAGS=-Wall -Ofast -pthread
INCLUDES=-I/opt/X11/include
LDFLAGS=-L/opt/X11/lib -lX11 -lm -lpthread 
CC=gcc

gameOfLife: gameOfLife.o graphics.o
	$(CC) -o game gameOfLife.o graphics.o $(LDFLAGS)

gameOfLife.o: gameOfLife.c
	$(CC) -g $(CFLAGS) $(INCLUDES) -c gameOfLife.c

graphics.o: graphics.h graphics.c
	$(CC) -g $(CFLAGS) $(INCLUDES) -c graphics.c

clean:
	rm -f game *.o