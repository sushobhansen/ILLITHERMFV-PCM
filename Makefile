#Makefile for ILLITHERM-FV

PROGRAM = ilthpcm
SOURCE = ilthpcm.cpp readinputs.cpp mesh.cpp surfaceflux.cpp solution.cpp MEPDG.cpp pcmfunctions.cpp
OBJECTS = ilthpcm.o readinputs.o mesh.o surfaceflux.o solution.o MEPDG.o pcmfunctions.o
ARCHIVE = ilthpcm.tar

CC = g++
CFLAGS = -g -lm -I.
DEPS = ilthpcm.h
OPTIONS = -std=c++11
#OPTIONS = -std=c++11 -g -debug extended -traceback -O0

%.o: %.cpp $(DEPS)
	$(CC) $(OPTIONS) -c -o $@ $< $(CFLAGS)

$(PROGRAM):	$(OBJECTS)
	$(CC) $(OPTIONS) -o $(PROGRAM).out $(OBJECTS) $(CFLAGS)
	
clean: 
	rm -f $(OBJECTS) $(PROGRAM).out *.tar *.plt
	
rebuild:
	clean $(PROGRAM)
	
archive:
	@echo Creating code archive tar-file $(ARCHIVE) ...
	tar cf $(ARCHIVE) $(SOURCE) Makefile
	@ls -l $(ARCHIVE)

help:
	@echo Try:
	@echo make $(PROGRAM) .... to build the program named $(PROGRAM)
	@echo make clean .... to clean up, removing object files and program $(PROGRAM)
	@echo make archive .... to make an archive tar file you can transfer or submit