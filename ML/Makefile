BINDIR = bin
OBJDIR = .obj
CXX = g++
CXXFLAGS = -g -O2 -Wall
LDFLAGS = -lgsl -lgslcblas
$(shell mkdir -p $(BINDIR) $(OBJDIR) > /dev/null)

SRCS = $(wildcard *.cpp)
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))
DEPS = $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.d))

.PHONY : all clean

all : $(BINDIR)/QrngTest

$(BINDIR)/QrngTest : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LDFLAGS)

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -MT $@ -MP -MM -MF $(patsubst %.o, %.d, $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

clean :
	rm -rf $(BINDIR) $(OBJDIR)

