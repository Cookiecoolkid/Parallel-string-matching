CXX = g++

CXXFLAGS = -std=c++17 -fopenmp -g -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -fstack-protector-all

SRCS = $(wildcard src/*.cpp)

OBJDIR = build

# Generate object files list
OBJS = $(patsubst src/%.cpp, $(OBJDIR)/%.o, $(SRCS))

# Generate target executables list
TARGETS = $(patsubst src/%.cpp, %, $(SRCS))

all: $(TARGETS)

# Rule to create each executable
%: $(OBJDIR)/%.o
	$(CXX) $(CXXFLAGS) $< -o $@

# Rule to compile each source file to object file
$(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create the build directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJS) $(TARGETS) $(OBJDIR)

run: all
	@for target in $(TARGETS); do \
		echo "Running $$target:"; \
		./$$target; \
	done

.PHONY: all clean run