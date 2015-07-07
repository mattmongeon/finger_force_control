PROG=biotac_cal
CC=g++
CPPFLAGS=-c -Wall
SOURCES=main.cpp pic_serial.cpp load_cell.cpp utils.cpp stopwatch.cpp keyboard_thread.cpp biotac.cpp
SOURCES+=real_time_plot.cpp data_logger.cpp
LFLAGS=-lpthread
LINKDIR=-Lusr/lib/x86_64-linux-gnu
PKGS=`pkg-config --cflags --libs plplot-c++`
OBJECTS=$(SOURCES:.cpp=.o)
OBJDIR=obj
BUILDOBJECTS=$(patsubst %,$(OBJDIR)/%,$(OBJECTS))

$(PROG): $(OBJECTS)
	$(CC) -o $(PROG) $(BUILDOBJECTS) $(LFLAGS) $(LINKDIR) $(PKGS)

%.o: %.cpp
	$(CC) $(CPPFLAGS) -o $(OBJDIR)/$@ $<

clean:
	rm -rf *.o biotac_cal *~ $(OBJDIR)
	mkdir $(OBJDIR)

rebuild: clean $(PROG)
