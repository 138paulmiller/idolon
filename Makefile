
SRC = $(wildcard code/*.cpp)
INC = -Icode/external
LIB = -pthread -lm -ldl -lSDL2


all: 
	g++ -std=c++14 $(INC) $(SRC) -w $(LIB) -o idolon

lint:
	clang-tidy --fix  $(SRC) -- $(INC) -w $(LIB)

install:
	$(shell apt-get install libsdl2-dev)
