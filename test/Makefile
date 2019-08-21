
CATCH2_DIR=deps/Catch2/single_include/

INC=-I$(CATCH2_DIR) -I../include

CXX=g++

all:
	$(CXX) -g -std=c++17 -Wall -Wextra -pedantic -o test-random test-random.cpp $(INC)

deps:
	mkdir -p -v deps 
	cd deps && git clone https://github.com/catchorg/Catch2.git
