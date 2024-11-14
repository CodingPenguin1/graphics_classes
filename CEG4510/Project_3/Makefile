CXXFLAGS=-g -m64 -Wno-deprecated $(DEFINES) -I/usr/X11R6/include -I/usr/local/include -std=c++11 -ggdb -Wall -Wextra -Wpedantic
LIBS=-L/usr/X11R6/lib64 -lglut -lGLU -lGL -lstdc++ -lXmu -lXi

FILES=main.cpp vector.cpp component.cpp

all: project3
plyfile.o: plyfile.cpp
	g++ -c plyfile.cpp $(LIBS) $(CXXFLAGS)

project3: $(FILES) plyfile.o
	g++ -o project3 $(FILES) plyfile.o $(LIBS) $(CXXFLAGS)
