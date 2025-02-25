CXXFLAGS = -std=c++11 -Werror -Wextra -Wall -pedantic -I.

all: proj1

proj1: proj1.o
	g++ $(CXXFLAGS) -o proj1 proj1.o

proj1.o: proj1.cc polyread.cc

clean:
	rm -f *.o proj1
