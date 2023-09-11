# Compiler and compiler flags
CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic -I./src -I/usr/include/boost

DEBUG_CXXFLAGS := $(CXXFLAGS) \
	-g3 \
	-ggdb \
	-O0 \
	-fsanitize=address,undefined \
	-fno-inline \
	-fvar-tracking \
	-ftrapv \
	-fno-eliminate-unused-debug-types \
	-fdiagnostics-color=auto

# Directories
SRCDIR := src
BUILDDIR := build
BINDIR := bin

# Source files and headers
HEADERS := $(shell find $(SRCDIR) -type f -name "*.h")
SOURCES := $(shell find $(SRCDIR) -type f -name "*.cpp")

# Object files
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))

# Executable
EXECUTABLE := $(BINDIR)/gcp

INPUT_FILE :=

# Targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lboost_program_options

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

debug: CXXFLAGS := $(DEBUG_CXXFLAGS)
debug: all
	@if [ -z "$(INPUT_FILE)" ]; then \
		echo "Usage: make debug INPUT_FILE=<input_file>"; \
	else \
		gdb -ex "break main" -ex "layout src" -ex run --args $(EXECUTABLE) --input $(INPUT_FILE); \
	fi

clean:
	@rm -rf $(BUILDDIR) $(BINDIR)

.PHONY: all clean debug
