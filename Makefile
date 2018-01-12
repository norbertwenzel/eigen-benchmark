CXXFLAGS=-DNONIUS_RUNNER -std=c++11 -Wall -Werror -pedantic -pthread
INCLUDES=-I /usr/include/eigen3/ -I ./external/
NAME=loop_bench
NAME-DEBUG=$(NAME)-debug

DEBUG_FLAGS=-O0
RELEASE_FLAGS=-O3 -march=native -mtune=native -DNDEBUG

all: debug release

debug: eigen-pttrans-bench.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(DEBUG_FLAGS) eigen-pttrans-bench.cpp -o $(NAME-DEBUG)

release: eigen-pttrans-bench.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $(RELEASE_FLAGS) eigen-pttrans-bench.cpp -o $(NAME)

bench: release
	./$(NAME) -s 1000

clean:
	rm -f $(NAME) $(NAME-DEBUG)
