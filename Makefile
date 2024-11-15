# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -Wpedantic -Wformat-security -Wconversion -Werror -O2 -std=c++11

# Target executable
TARGET = tp1

# Source files
SRC = teste.cpp

# Build all
all: $(TARGET)

# Build target
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Clean up
clean:
	rm -f $(TARGET)
