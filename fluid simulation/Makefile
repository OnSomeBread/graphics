CXXFLAGS= -std=c++14 -Wextra -Wall -pedantic -IC:/visual/graphics/include/
LIBS = -lglfw3 -lopengl32 -lgdi32
INCLUDES = -IC:/visual/graphics/include/glad/include -IC:/visual/graphics/include/glfw-3.4.bin.WIN64/include 
LDFLAGS = C:/visual/graphics/include/glad/src/glad.c -LC:/visual/graphics/include/glfw-3.4.bin.WIN64/lib-mingw-w64
all: water

water: 
	g++  ${CXXFLAGS} ${INCLUDES} water.cc -o water ${LDFLAGS} ${LIBS}

clean:
	rm -f *.o water
