# Define the compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g

# Define directories
SRC_DIR = source
INC_DIR = include
OBJ_DIR = obj

# Define the executable output
TARGET = client

# Define the source files
SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/utils.cpp $(SRC_DIR)/filehandler.cpp $(SRC_DIR)/messagehandler.cpp

# Define object files (compiled source files)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Create obj directory if it doesn't exist
$(shell mkdir -p $(OBJ_DIR))

# Default target to build the executable
all: $(TARGET)

# Rule to link the object files into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) -lpthread

# Rule to compile .cpp files to .o object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

# Clean up object files and executable
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Phony targets (not files)
.PHONY: all clean
