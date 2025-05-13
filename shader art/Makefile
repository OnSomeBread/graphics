CXXFLAGS= -std=c++14 -O3 -Wextra -Wall -pedantic -I../include/
LIBS = -lglfw3 -lopengl32 -lgdi32
INCLUDES = -I../include/glad/include -I../include/glfw-3.4.bin.WIN64/include 
LDFLAGS = ../include/glad/src/glad.c -L../include/glfw-3.4.bin.WIN64/lib-mingw-w64
all: shaders

shaders:
	g++  ${CXXFLAGS} ${INCLUDES} shaders.cc -o shaders ${LDFLAGS} ${LIBS}

clean:
	rm -f *.o shaders
