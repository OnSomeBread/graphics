# export CXXFLAGS="-std=c++11"
# g++ -o rd_view libcs630.a proj3.o -lX11 -lm 
# -Werror -Wextra -Wall -pedantic 
CXXFLAGS= -std=c++14 -O3 

water: water.o
	g++ -c -I. water.cc

clean:
	rm -f *.o water
