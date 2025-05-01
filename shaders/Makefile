CXXFLAGS= -std=c++14 -O3 -Wextra -Wall -pedantic
LIBS = -lglfw3 -lopengl32 -lgdi32
INCLUDES = -IC:/visual/glad/include -IC:/visual/glfw-3.4.bin.WIN64/include
LDFLAGS = C:/visual/glad/src/glad.c -LC:/visual/glfw-3.4.bin.WIN64/lib-mingw-w64
all: shaders

shaders:
	g++  ${CXXFLAGS} ${INCLUDES} shaders.cc -o shaders ${LDFLAGS} ${LIBS}

clean:
	rm -f *.o shaders
