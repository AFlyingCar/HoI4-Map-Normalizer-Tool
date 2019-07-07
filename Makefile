
CXX=clang++

BUILD_DIR=build
OUT_DIR=bin
SRC_DIR=src
INC_DIR=inc

SOURCES=$(SRC_DIR)/BitMap.cpp $(SRC_DIR)/main.cpp $(SRC_DIR)/ShapeFinder.cpp $(SRC_DIR)/GraphicalDebugger.cpp $(SRC_DIR)/UniqueColorGenerator.cpp $(SRC_DIR)/ProvinceMapBuilder.cpp $(SRC_DIR)/Util.cpp
OBJECTS=$(BUILD_DIR)/BitMap.o $(BUILD_DIR)/main.o $(BUILD_DIR)/ShapeFinder.o $(BUILD_DIR)/GraphicalDebugger.o $(BUILD_DIR)/UniqueColorGenerator.o $(BUILD_DIR)/ProvinceMapBuilder.o $(BUILD_DIR)/Util.o $(BUILD_DIR)/ColorArray.o
INCLUDES=$(INC_DIR)/BitMap.h $(INC_DIR)/ShapeFinder.h $(INC_DIR)/GraphicalDebugger.h $(INC_DIR)/UniqueColorGenerator.h $(INC_DIR)/ProvinceMapBuilder.h $(INC_DIR)/Util.h

# TODO: Add support for other architectures
ASM_SOURCES=$(SRC_DIR)/ColorArray_x86.asm

COLOR_GEN=color_generator.py

ifndef NODEBUG
DEBUG_FLAG=-g
else
DEBUG_FLAG=-O3 -flto=thin
LFLAGS=-flto=thin #-fuse-ld=gold
endif

WFLAGS=-Wall -Werror -Wextra -Wno-unused-command-line-argument -Wno-sign-compare -Wno-unused-label -Wno-unused-const-variable -Wno-unused-parameter

ifeq ($(OS),Windows_NT)
SDL_CFLAGS=-I./SDL/ -D_REENTRANT
SDL_LFLAGS=-lSDL2 -lsdl2main

CXXFLAGS+=-Xclang -flto-visibility-public-std -m32
LFLAGS+=-m32

PYTHON=python

else
SDL_CFLAGS:=`sdl2-config --cflags`
SDL_LFLAGS:=`sdl2-config --libs`

PYTHON=python3

endif

ENABLE_GRAPHICS_C=-DENABLE_GRAPHICS $(SDL_CFLAGS)
ENABLE_GRAPHICS_L=-DENABLE_GRAPHICS $(SDL_LFLAGS)

CPP_VER=-std=c++17

INCFLAGS+=-I$(INC_DIR)/
CXXFLAGS+=$(DEBUG_FLAG) $(ENABLE_GRAPHICS_C) $(CPP_VER) $(INCFLAGS) $(WFLAGS)
LFLAGS+=-pthread $(ENABLE_GRAPHICS_L)

OUT=$(OUT_DIR)/fp

all: $(OBJECTS) $(OUT_DIR)/ color_files
	$(CXX) $(LFLAGS) $(OBJECTS) -o $(OUT)

clean:
	rm $(OBJECTS) $(OUT)

$(BUILD_DIR)/BitMap.o: $(INC_DIR)/BitMap.h $(BUILD_DIR)/
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/BitMap.cpp -o $(BUILD_DIR)/BitMap.o

$(BUILD_DIR)/main.o: $(BUILD_DIR)/
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/ShapeFinder.o: $(INC_DIR)/ShapeFinder.h $(BUILD_DIR)/
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/ShapeFinder.cpp -o $(BUILD_DIR)/ShapeFinder.o

$(BUILD_DIR)/GraphicalDebugger.o: $(INC_DIR)/GraphicalDebugger.h $(BUILD_DIR)/
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/GraphicalDebugger.cpp -o $(BUILD_DIR)/GraphicalDebugger.o

$(BUILD_DIR)/UniqueColorGenerator.o: $(INC_DIR)/UniqueColorGenerator.h $(BUILD_DIR)/
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/UniqueColorGenerator.cpp -o $(BUILD_DIR)/UniqueColorGenerator.o

$(BUILD_DIR)/ProvinceMapBuilder.o: $(INC_DIR)/ProvinceMapBuilder.h $(BUILD_DIR)/
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/ProvinceMapBuilder.cpp -o $(BUILD_DIR)/ProvinceMapBuilder.o

$(BUILD_DIR)/Util.o: $(INC_DIR)/Util.h $(BUILD_DIR)/
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/Util.cpp -o $(BUILD_DIR)/Util.o

$(BUILD_DIR)/ColorArray.o: $(INC_DIR)/ColorArray.h color_files $(BUILD_DIR)/
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/ColorArray_x86.asm -o $(BUILD_DIR)/ColorArray.o

color_files: $(COLOR_GEN)
	$(PYTHON) $(COLOR_GEN)

$(OUT_DIR)/:
	mkdir -p $(OUT_DIR)

$(BUILD_DIR)/:
	mkdir -p $(BUILD_DIR)

