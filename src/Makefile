CXX = g++
CXXFLAGS = -g -std=c++11 -Wall -pedantic -O3 -Werror
BIN = mwm_diff

SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=%.o)

all: $(OBJ)
	$(CXX) -o $(BIN) $^

%.o: %.c
	$(CXX) $@ -c $<

clean:
	rm -f *.o
	rm -f $(BIN)
