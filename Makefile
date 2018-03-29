CC = g++
CFLAGS = -std=c++0x

OBJS = main.o

all: main

main: $(OBJS)


main.o : main.cpp
	$(CC) $(CFLAGS) -c main.cpp

clean:
	rm -f *~ *.o main



