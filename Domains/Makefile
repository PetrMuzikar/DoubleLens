CPP := g++ -g -Wall
RM := rm -f

PROG := Domains
SRC_BASE := Main
SRC := $(addsuffix .cpp, $(SRC_BASE))
HEADERS := Domain.hpp Rectangle.hpp
OBJS := $(addsuffix .o, $(SRC_BASE))

ALL : $(PROG)

Domains : $(OBJS)
	$(CPP) -o $@ $+

#Rectangle.o : Rectangle.cpp $(HEADERS)
#	$(CPP) -c -o $@ $<

Main.o : Main.cpp $(HEADERS)
	$(CPP) -c -o $@ $<

CLEAN :
	$(RM) $(OBJS) $(PROG)
