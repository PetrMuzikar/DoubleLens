OBJDIR = .obj
CXX = g++
CXXFLAGS = -g -Wall
$(shell mkdir -p $(OBJDIR) > /dev/null)

SRCS = $(wildcard *.cpp)
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))
DEPS = $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.d))

.PHONY : all clean

all : MLTest

MLTest : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LDFLAGS)

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -MT $@ -MP -MM -MF $(patsubst %.o, %.d, $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

clean :
	rm -rf $(OBJDIR)

