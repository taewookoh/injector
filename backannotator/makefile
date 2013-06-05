all: dwarfparser

INCLUDES=-I/home/twoh/local/include 
LDFLAGS=-L/home/twoh/local/lib
LIBS=-lelf -ldwarf

dwarfparser: dwarfparser.cpp
	g++ -O3 -o $@ $< $(INCLUDES) $(LDFLAGS) $(LIBS)
