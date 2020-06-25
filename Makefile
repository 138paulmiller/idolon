
SRC = $(wildcard code/*.cpp)
INC = -Icode/external
LIB = -pthread -lm -ldl -lSDL2


install:
	$(shell apt-get install libsdl2-dev)

all: 
	g++ -std=c++14 $(INC) $(SRC) -w $(LIB) -o ultboy

