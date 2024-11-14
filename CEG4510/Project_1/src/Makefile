
OBJECTS=main.o plyfile.o

DEFINES=-DEMULATE_STEREO
CXXFLAGS=-g -m64 -Wno-deprecated $(DEFINES) -I/usr/X11R6/include -I/usr/local/include

LIBS=-L/usr/X11R6/lib64 -lglut -lGLU -lGL -lstdc++ -lXmu -lXi

all: project1

clean:
	rm $(OBJECTS)
	rm project1

project1: $(OBJECTS)
	g++ -o project1 $(OBJECTS) $(LIBS)

.SUFFIXES: .cpp

%.o : %.ccp
	g++ -c $(CXXFLAGS) -o $@ $<
