CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -pedantic -pthread -I./include
LDFLAGS = -pthread

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
MAIN_OBJ = $(BUILD_DIR)/main_game.o
LIB_OBJECTS = $(filter-out $(MAIN_OBJ),$(OBJECTS))

TARGET = $(BIN_DIR)/minimax_game

SEED ?= 42
PORT ?= 5555
ADDR ?= 127.0.0.1

.PHONY: all clean dirs host join

all: dirs $(TARGET)

dirs:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Run as host (Terminal 1)
host: $(TARGET)
	@echo "Starting game as HOST on port $(PORT) with seed $(SEED)..."
	./$(TARGET) --mode host --seed $(SEED) --port $(PORT)

# Run as client (Terminal 2)
client: $(TARGET)
	@echo "Joining game as CLIENT at $(ADDR):$(PORT)..."
	./$(TARGET) --mode join --addr $(ADDR) --port $(PORT)

help:
	@echo "Usage:"
	@echo "make clean"
	@echo "make"
	@echo ""
	@echo "make host"
	@echo "make client"
	@echo "make host SEED=42 PORT=5555"
	@echo "make join ADDR=127.0.0.1 PORT=5555"
