# Compiler
CXX := g++

# Compiler flags
CXXFLAGS := -Wall -Wextra -std=c++20 -Iinclude

# Directories
SRC_DIR := .
BUILD_DIR := ./build
INCLUDE_DIR := .

# Find all .cpp files in the src directory
SRCS := $(wildcard $(SRC_DIR)/*.cpp)

# Generate object files in the build directory
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Executable name
TARGET := main

# Default target
all: $(BUILD_DIR)/$(TARGET)

# Rule to link object files to create the executable
$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@

# Rule to compile .cpp files into .o files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target to remove built files
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean

