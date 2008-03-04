CXX=g++
OBJS=main.o peer.o fake_console.o
PREFIX=/usr/local
INCLUDE=${PREFIX}/include
CFLAGS=-O0 -g3 -Wall -ansi -pedantic

all: main

main: ${OBJS}
	${CXX} -o main ${OBJS} -lzigd

main.o: main.cpp
	${CXX} ${CFLAGS} -I${INCLUDE} -c main.cpp

peer.o: peer.cpp
	${CXX} ${CFLAGS} -I${INCLUDE} -c peer.cpp


fake_console.o: fake_console.cpp
	${CXX} ${CFLAGS} -I${INCLUDE} -c fake_console.cpp

clean:
	rm -f main main.o peer.o fake_console.o
