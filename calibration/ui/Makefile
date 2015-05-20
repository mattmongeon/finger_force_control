PROG=biotac_cal
CC=g++
CPPFLAGS=-c -Wall
SOURCES=main.cpp pic_comm.cpp load_cell_cal.cpp utils.cpp
OBJECTS=$(SOURCES:.cpp=.o)

$(PROG): $(OBJECTS)
	$(CC) -o $(PROG) $(OBJECTS) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CPPFLAGS) -o $@ $<

clean:
	rm -rf *.o biotac_cal *~

rebuild: clean $(PROG)
