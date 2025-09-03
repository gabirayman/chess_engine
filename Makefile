CXX      ?= g++
CXXFLAGS ?= -std=c++20 -O2 -Wall -Wextra -Wpedantic -Iinclude
LDFLAGS  ?=

SRCS := $(wildcard src/*.cpp)
OBJS := $(SRCS:src/%.cpp=build/%.o)
DEPS := $(OBJS:.o=.d)


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

-include $(DEPS)