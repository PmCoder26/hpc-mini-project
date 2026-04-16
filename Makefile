CXX = clang++
# Default macOS clang flags for OpenMP. Note: You must have libomp installed (`brew install libomp`)
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -Xpreprocessor -fopenmp
# Update these paths if your brew installs libomp somewhere else, like /opt/homebrew
LDFLAGS = -lomp -L/usr/local/opt/libomp/lib -L/opt/homebrew/opt/libomp/lib
INC_FLAGS = -Iinclude -I/usr/local/opt/libomp/include -I/opt/homebrew/opt/libomp/include

SRC_DIR = src
BUILD_DIR = build

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

TARGET = hpc_engine

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)/* $(TARGET)

run: $(TARGET)
	./$(TARGET)
