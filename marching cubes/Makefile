CXXFLAGS = -std=c++11 -Werror -Wextra -Wall -pedantic -I.

all: proj2 cview

proj2: proj2.o
	g++ $(CXXFLAGS) -o proj2 proj2.o

proj2.o: proj2.cc proj2.h

cview: cview.o
	g++ $(CXXFLAGS) -o cview cview.o

clean:
	rm -f *.o proj2 cview
