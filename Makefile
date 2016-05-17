MKDIR = mkdir -p
RM = rm -f
CXX = g++
CXXFLAGS = -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wconversion
LDLIBS = -lpthread -lX11 -lHalf -lIlmImf
SRCS = main.cpp Panorama.cpp Tonemapper.cpp HDR.cpp
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

$(OBJ_DIR)/Panorama.o: $(SRC_DIR)/Panorama.cpp $(OBJ_DIR)/Tonemapper.o $(OBJ_DIR)/HDR.o
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/Panorama.o -c $(SRC_DIR)/Panorama.cpp $(LDLIBS)

$(OBJ_DIR)/HDR.o: $(SRC_DIR)/HDR.o
	$(MKDIR) $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/HDR.o -c $(SRC_DIR)/HDR.cpp $(LDLIBS)

$(OBJ_DIR)/Tonemapper.o: $(SRC_DIR)/Tonemapper.o
	$(MKDIR) $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/Tonemapper.o -c $(SRC_DIR)/Tonemapper.cpp $(LDLIBS)

clean:
	$(RM) $(EXE) $(OBJS)
