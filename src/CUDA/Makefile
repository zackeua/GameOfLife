CFLAGS=-g
INCLUDES=-I/opt/X11/include
LDFLAGS=-L/opt/X11/lib -lX11 -lm -lpthread 
CC=nvcc

gameOfLife: game.o
	$(CC) -o game game.o $(LDFLAGS)

game.o: game.cu
	$(CC) $(CFLAGS) $(INCLUDES) -c game.cu

clean:
	rm -f game *.o