PROG=biotac_cal
CC=g++
CPPFLAGS=-c -Wall -fopenmp
INCLUDES=-I/usr/local/include/eigen3

SOURCES=main.cpp pic_serial.cpp load_cell.cpp utils.cpp stopwatch.cpp keyboard_thread.cpp biotac.cpp
SOURCES+=real_time_plot.cpp data_logger.cpp file_plotter.cpp data_file_reader.cpp function_fit_nn.cpp
SOURCES+=file_utils.cpp function_fit_nls.cpp electrode_tdc_compensator.cpp function_fit_force_terms.cpp
SOURCES+=biotac_force_curve.cpp data_file_editor.cpp motor.cpp file_system.cpp electrode_comp_tare.cpp
SOURCES+=biotac_force_curve_tare.cpp function_fit_tare.cpp

LFLAGS=-lboost_thread -lboost_system -lboost_timer -lboost_filesystem -lboost_chrono 
LFLAGS+=-lceres -lgflags -lglog -llapack -lcamd -lamd -lccolamd -lcolamd -lcholmod
LFLAGS+=-lcxsparse -fopenmp -lpthread -lgomp -lm -lblas
LINKDIR=-Lusr/lib/x86_64-linux-gnu
PKGS=`pkg-config --cflags --libs plplot-c++` `pkg-config --cflags --libs fann`

OBJECTS=$(SOURCES:.cpp=.o)
OBJDIR=obj
BUILDOBJECTS=$(patsubst %,$(OBJDIR)/%,$(OBJECTS))
HDRS := $(wildcard *.h)


$(PROG): $(BUILDOBJECTS)
	$(CC) -o $(PROG) $(BUILDOBJECTS) $(LFLAGS) $(LINKDIR) $(PKGS)

$(OBJDIR)/%.o: %.cpp
	$(CC) $(CPPFLAGS) $(INCLUDES) -o $@ $<

.PHONY: clean
clean:
	rm -rf *.o biotac_cal *~ $(OBJDIR)
	mkdir $(OBJDIR)

.PHONY: rebuild
rebuild: clean $(PROG)
