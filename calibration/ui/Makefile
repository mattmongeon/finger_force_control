PROG=biotac_cal
CC=g++
CPPFLAGS=-c -Wall
SOURCES=main.cpp pic_serial.cpp load_cell.cpp utils.cpp stopwatch.cpp keyboard_thread.cpp
LFLAGS=-lpthread
LINKDIR=-Lusr/lib/x86_64-linux-gnu
PKGS=`pkg-config --cflags --libs plplot-c++`
OBJECTS=$(SOURCES:.cpp=.o)

$(PROG): $(OBJECTS)
	$(CC) -o $(PROG) $(OBJECTS) $(LFLAGS) $(LINKDIR) $(PKGS)

%.o: %.cpp
	$(CC) $(CPPFLAGS) -o $@ $<

clean:
	rm -rf *.o biotac_cal *~

rebuild: clean $(PROG)
