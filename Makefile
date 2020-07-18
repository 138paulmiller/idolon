OUT = idolon
ROOT = code
CXX = clang++
PYTHON = python-3.4
# -H 
FLAGS =-std=c++14  -O2 -Wall 
SRC = $(wildcard $(ROOT)/*.cpp)
LIB_INC = -I$(ROOT)/external $(shell pkg-config $(PYTHON) --cflags)
LIB = -lm -ldl -lSDL2 $(shell pkg-config $(PYTHON) --libs)
PCH = $(ROOT)/pch.hpp
PCH_OUT = $(basename $(PCH)).gen.hpp


all: $(PCH_OUT)
	$(CXX) $(FLAGS) -include-pch $(PCH_OUT) $(LIB_INC) $(SRC) -w $(LIB) -o $(OUT)

# Compiles your PCH
$(PCH_OUT): $(PCH)
	$(CXX) $(FLAGS) $(LIB_INC) $< -o $@ 

clean:
	rm $(PCH_OUT)

lint:
	clang-tidy --fix  $(SRC) -- $(LIB_INC)

install:
	$(shell apt-get install libsdl2-dev clang python3.4-dev)





