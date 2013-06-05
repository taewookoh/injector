all: dwarfparser

INCLUDES=-I/home/twoh/local/include 
LDFLAGS=-L/home/twoh/local/lib
LIBS=-ldwarf -lelf

dwarfparser: dwarfparser.cpp
	g++ -O3 -o $@ $< $(INCLUDES) $(LDFLAGS) $(LIBS)

clean:
	rm -f dwarfparser
