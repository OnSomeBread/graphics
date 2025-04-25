CXXFLAGS= -std=c++14 -O3 -Wextra -Wall -pedantic -I.
all: water

water: water.o
	g++  ${CXXFLAGS} -o water water.o

water.o: water.cc water.h

clean:
	rm -f *.o water
