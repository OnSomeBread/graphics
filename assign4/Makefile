# export CXXFLAGS="-std=c++11"
# g++ -o rd_view libcs630.a proj3.o -lX11 -lm 
CXXFLAGS="-std=c++11"
rdv: libcs630.a proj4.o
	g++ -o rdv libcs630.a proj4.o -I /opt/X11/include -L /opt/X11/lib -lX11 -lm 

proj3: proj4.o
	g++ -c -I. proj4.cc proj4.h -Werror -Wextra -Wall -pedantic 

clean:
	rm -f *.o proj4
	rm rdv
