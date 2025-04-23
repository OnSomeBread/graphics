# export CXXFLAGS="-std=c++11"
# g++ -o rd_view libcs630.a proj3.o -lX11 -lm 
# -Werror -Wextra -Wall -pedantic 
CXXFLAGS= -std=c++14 -O3 
all: rdv start_water grass smoke

rdv: libcs630.a proj4.o
	g++ -o rdv libcs630.a proj4.o -I /opt/X11/include -L /opt/X11/lib -lX11 -lm 

proj4: proj4.o
	g++ -c -I. proj4.cc

clean:
	rm -f *.o proj4 smoke grass water rdv
