OUT = idolon
ROOT = code
CXX = clang++
# -H 
FLAGS = -O2 -Wall -std=c++14
SRC = $(wildcard $(ROOT)/*.cpp)
LIB_INC = -I$(ROOT)/external
LIB = -lm -ldl -lSDL2
PCH = $(ROOT)/pch.h
PCH_OUT = $(PCH).gch


all: $(PCH_OUT)
	$(CXX) $(FLAGS) -include-pch $(PCH_OUT) $(LIB_INC) $(SRC) -w $(LIB) -o $(OUT)

# Compiles your PCH
$(PCH_OUT): $(PCH)
	$(CXX) $(FLAGS) $(LIB_INC) $< -o $@

clean:
	rm $(PCH_OUT)

lint:
	clang-tidy --fix  $(SRC) -- $(LIB_INC) -w $(LIB)

install:
	$(shell apt-get install libsdl2-dev clang)





