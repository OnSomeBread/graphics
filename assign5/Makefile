CXXFLAGS = -std=c++11 -Werror -Wextra -Wall -pedantic -I.

proj5: proj5.o
	g++ $(CXXFLAGS) -o proj5 proj5.o

proj5.o: proj5.cc proj5.h

clean:
	rm -f *.o proj5
