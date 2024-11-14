CXXFLAGS=-g -m64 -Wno-deprecated $(DEFINES) -I/usr/X11R6/include -I/usr/local/include -std=c++11 -ggdb -Wall -Wextra -Wpedantic
LIBS=-L/usr/X11R6/lib64 -lglut -lGLU -lGL -lstdc++ -lXmu -lXi

FILES=main.cpp vector.cpp sphere.cpp

all: project4

project4: $(FILES)
	g++ -o project4 $(FILES) $(LIBS) $(CXXFLAGS)
