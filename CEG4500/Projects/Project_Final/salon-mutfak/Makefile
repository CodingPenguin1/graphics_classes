OJECTS=salon-mutfak.o trackball.o main.o

CXX=g++
CXXFLAGS=-g -Wno-deprecated -Wno-write-strings -I/usr/X11R6/include -I/usr/local/include -I./src

LIBS=-L/usr/X11R6/lib64 -lglui -lglut -lGLEW -lGLU -lGL -lstdc++ -lXmu -lXi

all: salonmutfak

.SUFFIXES: .cpp .o

.cpp.o:
	$(CXX) $(CFLAGS) -c $<

clean:
	rm $(OBJECTS)
	rm salonmutfak

salonmutfak: $(OBJECTS)
	$(CXX) -g -o salonmutfak $(OBJECTS) $(LIBS)
