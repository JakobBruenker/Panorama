MKDIR = mkdir -p
RM = rm -f
CXX = g++
CXXFLAGS = -Wall
LDLIBS = -lpthread -lX11 -lHalf -lIlmImf
SRCS = main.cpp Panorama.cpp
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
EXE = $(BIN_DIR)/panorama

all: run

run: $(EXE)
	$(EXE)

build: $(EXE)

$(EXE): $(OBJS)
	$(MKDIR) $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(OBJS) $(LDLIBS)

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp $(OBJ_DIR)/Panorama.o
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/main.o -c $(SRC_DIR)/main.cpp $(LDLIBS)

$(OBJ_DIR)/Panorama.o: $(SRC_DIR)/Panorama.cpp
	$(MKDIR) $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/Panorama.o -c $(SRC_DIR)/Panorama.cpp $(LDLIBS)

clean:
	$(RM) $(EXE) $(OBJS)