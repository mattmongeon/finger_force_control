PROG=biotac_cal
CC=g++
CPPFLAGS=-c -Wall
SOURCES=main.cpp pic_serial.cpp load_cell.cpp utils.cpp
OBJECTS=$(SOURCES:.cpp=.o)

$(PROG): $(OBJECTS)
	$(CC) -o $(PROG) $(OBJECTS) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CPPFLAGS) -o $@ $<

clean:
	rm -rf *.o biotac_cal *~

rebuild: clean $(PROG)
