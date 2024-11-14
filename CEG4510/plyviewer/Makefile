
OBJECTS=main.o \
	trackball.o \
	plyfile.o

DEFINES=-DEMULATE_STEREO
CXXFLAGS=-g -m64 -Wno-deprecated $(DEFINES) -I/usr/X11R6/include -I/usr/local/include

LIBS=/usr/local/lib64/libglui.a -L/usr/X11R6/lib64 -lglut -lGLU -lGL -lstdc++ -lXmu -lXi

all: plyviewer

clean:
	rm $(OBJECTS)
	rm plyviewer

plyviewer: $(OBJECTS)
	g++ -o plyviewer $(OBJECTS) $(LIBS)

.SUFFIXES: .cpp

%.o : %.ccp
	g++ -c $(CXXFLAGS) -o $@ $<
