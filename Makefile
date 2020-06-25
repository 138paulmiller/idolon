
SRC = $(wildcard code/*.cpp)
INC = -Icode/external
LIB = -pthread -lm -ldl -lSDL2 -lSDL_image


install:
	$(shell apt-get install libsdl2-dev libsdl-image1.2-dev)

all: 
	g++ -std=c++14 $(INC) $(SRC) -w $(LIB) -o ultboy

