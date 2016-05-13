MKDIR = mkdir -p
RM = rm -f
CXX = g++
CXXFLAGS = -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wconversion -Werror
LDLIBS = -lpthread -lX11
SRCS = main.cpp Panorama.cpp CDisplay.cpp
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

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp $(OBJ_DIR)/Panorama.o $(OBJ_DIR)/CDisplay.o
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/main.o -c $(SRC_DIR)/main.cpp $(LDLIBS)

$(OBJ_DIR)/Panorama.o: $(SRC_DIR)/Panorama.cpp $(OBJ_DIR)/CDisplay.o
	$(MKDIR) $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/Panorama.o -c $(SRC_DIR)/Panorama.cpp $(LDLIBS)

$(OBJ_DIR)/CDisplay.o: $(SRC_DIR)/Panorama.cpp
	$(MKDIR) $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/CDisplay.o -c $(SRC_DIR)/CDisplay.cpp $(LDLIBS)
clean:

	$(RM) $(EXE) $(OBJS)
