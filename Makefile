BINARIES = DoubleLensSobolSimple DoubleLensSobolInteg \
		   DoubleLensRandomSimple DoubleLensRandomInteg
BINDIR = bin
BIN = $(addprefix $(BINDIR)/, $(BINARIES))

CXX = g++
CXXFLAGS = -g -O2 -Wall -std=c++11 -I ML
CXXFLAGS_SobolSimple = -UDEBUG_MAIN -DSOBOL_RANDOM_SEQUENCE -DINTEGRATE_PHOTON_PATH
CXXFLAGS_SobolInteg = -UDEBUG_MAIN -DSOBOL_RANDOM_SEQUENCE
CXXFLAGS_RandomSimple = -UDEBUG_MAIN -DRANDOM_RAYS 
CXXFLAGS_RandomInteg = -UDEBUG_MAIN -DINTEGRATE_PHOTON_PATH -DRANDOM_RAYS
LDFLAGS = -lm -lgsl -lgslcblas

SRC_LENS = $(wildcard *.cpp) 
SRC_ML = $(filter-out ML/MLTest.cpp, $(wildcard ML/*.cpp))
SRC = $(SRC_LENS) $(SRC_ML)

OBJDIR_LENS = .obj
OBJDIR_ML = ./ML/.obj
OBJDIRS = $(addprefix $(OBJDIR_LENS)/, $(BINARIES))
OBJ_LENS = $(SRC_LENS:.cpp=.o)
OBJ_ML = $(addprefix $(OBJDIR_ML)/, $(notdir $(SRC_ML:.cpp=.o)))
OBJ = $(OBJ_LENS) $(OBJ_ML)

DEP_LENS = $(SRC_LENS:.cpp=.d)
DEP_ML = $(SRC_ML:.cpp=.d)
DEP = $(DEP_LENS) $(DEP_ML)

OBJ_SobolSimple = $(addprefix $(OBJDIR_LENS)/DoubleLensSobolSimple/, $(OBJ_LENS)) 
DEP_SobolSimple = $(addprefix $(OBJDIR_LENS)/DoubleLensSobolSimple/, $(DEP_LENS)) 

OBJ_SobolInteg = $(addprefix $(OBJDIR_LENS)/DoubleLensSobolInteg/, $(OBJ_LENS)) 
DEP_SobolInteg = $(addprefix $(OBJDIR_LENS)/DoubleLensSobolInteg/, $(DEP_LENS)) 

OBJ_RandomSimple = $(addprefix $(OBJDIR_LENS)/DoubleLensRandomSimple/, $(OBJ_LENS)) 
DEP_RandomSimple = $(addprefix $(OBJDIR_LENS)/DoubleLensRandomSimple/, $(DEP_LENS)) 

OBJ_RandomInteg = $(addprefix $(OBJDIR_LENS)/DoubleLensRandomInteg/, $(OBJ_LENS)) 
DEP_RandomInteg = $(addprefix $(OBJDIR_LENS)/DoubleLensRandomInteg/, $(DEP_LENS)) 

$(shell mkdir -p $(BINDIR) $(OBJDIRS) $(OBJDIR_ML) > /dev/null)

$(info $(OBJDIR_ML))
$(info $(OBJ_SobolSimple))

.PHONY : all clean

all : $(BIN)

#==========================================
bin/DoubleLensSobolSimple : $(OBJ_ML) $(OBJ_SobolSimple) 
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LDFLAGS)

.obj/DoubleLensSobolSimple/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_SobolSimple) -c $< -MT $@ -MP -MM -MF $(patsubst %.o, %.d, $@)
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_SobolSimple) -c $< -o $@

#==========================================
bin/DoubleLensSobolInteg : $(OBJ_ML) $(OBJ_SobolInteg) 
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LDFLAGS)

.obj/DoubleLensSobolInteg/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_SobolInteg) -c $< -MT $@ -MP -MM -MF $(patsubst %.o, %.d, $@)
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_SobolInteg) -c $< -o $@

#==========================================
bin/DoubleLensRandomSimple : $(OBJ_ML) $(OBJ_RandomSimple) 
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LDFLAGS)

.obj/DoubleLensRandomSimple/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_RandomSimple) -c $< -MT $@ -MP -MM -MF $(patsubst %.o, %.d, $@)
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_RandomSimple) -c $< -o $@

#==========================================
bin/DoubleLensRandomInteg : $(OBJ_ML) $(OBJ_RandomInteg) 
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LDFLAGS)

.obj/DoubleLensRandomInteg/%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_RandomInteg) -c $< -MT $@ -MP -MM -MF $(patsubst %.o, %.d, $@)
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_RandomInteg) -c $< -o $@

#==========================================

$(OBJDIR_ML)%.o : ML/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -MT $@ -MP -MM -MF $(patsubst %.o, %.d, $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEP)

clean :
	rm -rf $(BINDIR) $(OBJDIR_LENS) $(OBJDIR_ML)

