CXXFLAGS = -std=c++11 -Wextra -Wall -pedantic -I.

all: proj3

proj3: proj3.o
	g++ $(CXXFLAGS) -o proj3 proj3.o

proj3.o: proj3.cc cs630.h

clean:
	rm -f *.o proj3
