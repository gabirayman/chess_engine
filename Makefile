CXX      ?= g++
CXXFLAGS ?= -std=c++20 -O2 -Wall -Wextra -Wpedantic -Iinclude
LDFLAGS  ?=

SRCS := $(wildcard src/*.cpp)
OBJS := $(SRCS:src/%.cpp=build/%.o)
DEPS := $(OBJS:.o=.d)

CORE_SRCS := $(filter-out src/main.cpp, $(SRCS))

.PHONY: all
all: bin/chess

bin/chess: $(OBJS)
	@mkdir -p bin
	$(CXX) $(LDFLAGS) $^ -o $@

build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

.PHONY: run
run: bin/chess
	./bin/chess $(ARGS)

.PHONY: clean
clean:
	rm -rf build bin

# --- perft checker ---
.PHONY: perft-check run-perft-check depth4 depth5 depth4time depth5time

perft-check: bin/perft_check

bin/perft_check: $(CORE_SRCS) tests/perft_check.cpp
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $^ -o $@

# make run-perft-check ARGS='--file tests/data/perft_cases.txt --depth 4'
run-perft-check: bin/perft_check
	./bin/perft_check $(ARGS)

# shortcut to run at depth 4
depth4:
	$(MAKE) run-perft-check ARGS='--file tests/data/perft_cases.txt --depth 4'

# shortcut to run at depth 4 with timing
depth4time:
	$(MAKE) run-perft-check ARGS='--file tests/data/perft_cases.txt --depth 4 --time'

# shortcut to run at depth 5
depth5:
	$(MAKE) run-perft-check ARGS='--file tests/data/perft_cases.txt --depth 5'

# shortcut to run at depth 5
depth5time:
	$(MAKE) run-perft-check ARGS='--file tests/data/perft_cases.txt --depth 5 --time'

-include $(DEPS)