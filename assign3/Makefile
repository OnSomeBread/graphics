# export CXXFLAGS="-std=c++11"
# g++ -o rd_view libcs630.a proj3.o -lX11 -lm 
rdv: libcs630.a proj3.o
	g++ -o rdv libcs630.a proj3.o -I /opt/X11/include -L /opt/X11/lib -lX11 -lm 

proj3: proj3.o
	g++ -c -I. proj3.cc -Werror -Wextra -Wall -pedantic 

clean:
	rm -f *.o proj3
	rm rdv
