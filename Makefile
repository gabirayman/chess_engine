CXX      ?= g++
CXXFLAGS ?= -std=c++20 -O2 -Wall -Wextra -Wpedantic -Iinclude
LDFLAGS  ?=

SRCS := $(wildcard src/*.cpp)
OBJS := $(SRCS:src/%.cpp=build/%.o)
DEPS := $(OBJS:.o=.d)


.PHONY: all
all: bin/chess

# Link step
bin/chess: $(OBJS)
	@mkdir -p bin
	$(CXX) $(LDFLAGS) $^ -o $@

# Compile step (+ write .d dependency files)
build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Run it (pass args with ARGS='...')
.PHONY: run
run: bin/chess
	./bin/chess $(ARGS)

# Clean
.PHONY: clean
clean:
	rm -rf build bin

# Auto-include header dependencies (if present)
-include $(DEPS)