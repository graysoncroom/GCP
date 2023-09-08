# Compiler and compiler flags
CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -pedantic -I./src -I/usr/include/boost

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

# Targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lboost_program_options

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	@rm -rf $(BUILDDIR) $(BINDIR)

.PHONY: all clean
