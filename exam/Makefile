SRC = tests.cpp check_stack_pool.cpp check_const_iterator.cpp check_swap.cpp

CXX = c++
CXXFLAGS = -Wall -Wextra -std=c++14 -O3

EXE = $(SRC:.cpp=.x)

# eliminate default suffixes
.SUFFIXES:
SUFFIXES =

# just consider our own suffixes
.SUFFIXES: .cpp .x .o

all: $(EXE)

check: tests.x
	./$< -s

.PHONY: all

%.x:
	$(CXX) $^ -o $@

%.o: %.cpp 
	$(CXX) $< -o $@ $(CXXFLAGS) -c

format: $(SRC)
	@clang-format -i $^ -verbose || echo "Please install clang-format to run this command"

.PHONY: format

clean:
	rm -f $(EXE) *~ *.o

.PHONY: clean

documentation: Doxygen/doxyfile
	doxygen $^

.PHONY: documentation

tests.x: tests_main.o tests.o

tests.o: tests.cpp catch.hpp stack_pool.hpp

stack_pool.o: stack_pool.cpp

check_stack_pool.x: check_stack_pool.o

check_stack_pool.o: check_stack_pool.cpp stack_pool.cpp

check_const_iterator.x: check_const_iterator.o

check_const_iterator.o: check_const_iterator.cpp stack_pool.cpp

check_swap.x: check_swap.o

check_swap.o: check_swap.cpp

format : stack_pool.hpp

