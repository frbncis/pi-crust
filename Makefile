# build:
# 	mkdir -p out
# 	g++ -o out/kvm.out hid.c xreadkeys.cpp scancodes.cpp -DXK_MISCELLANY=1 -DXK_LATIN1=1 -L/usr/include/ -lX11

# .PHONY: clean

# https://dev.to/dmendoza99/how-to-write-a-good-makefile-for-c-32e3
# SDL include notes: https://forums.libsdl.org/viewtopic.php?t=5997

CXX				:= g++
CXX_FLAGS		:= -Wall -Wextra -std=c++17 -ggdb
LINKER_FLAGS	:= -DXK_MISCELLANY=1 -DXK_LATIN1=1
BIN				:= bin
SRC				:= src
INCLUDE			:= include
LIB				:= LIB
LIBRARIES		:= -lX11
EXECUTABLE		:= kvm


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	@echo "Executing..."
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	@echo "Building..."
	$(CXX) $^  $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) -o $@ $(LIBRARIES) $(LINKER_FLAGS)

clean:
	@echo "Cleaning..."
	-rm $(BIN)/*