CC = g++
CFLAGS = -std=c++11

OBJS = main.o

all: main

main: $(OBJS)


main.o : main.cpp
	$(CC) $(CFLAGS) -c main.cpp

clean:
	rm -f *~ *.o main



