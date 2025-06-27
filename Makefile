# Project Configuration
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build
BUILD_TEST_DIR = $(BUILD_DIR)/tests


# Executables
EXEC = $(BUILD_DIR)/main
TEST_EXEC = $(BUILD_TEST_DIR)/run_tests


# Source and Object Files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c,$(BUILD_TEST_DIR)/%.o,$(TEST_SRCS))


# Compiler Configuration
CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c2x -I$(SRC_DIR) -Iinclude
TEST_CFLAGS = $(CFLAGS) -I$(TEST_DIR)
TEST_LDFLAGS = -lcriterion


# Build Mode (debug by default)
DEBUG ?= 1
ifeq ($(DEBUG),1)
   CFLAGS += -g -DDEBUG
else
   CFLAGS += -O2 -DNDEBUG -s
   LDFLAGS += -s
endif


# Default Target
all: $(EXEC)


# Create build directories
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR) $(BUILD_TEST_DIR)


# Main Application
$(EXEC): $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^


# Test Executable
$(TEST_EXEC): $(filter-out $(BUILD_DIR)/main.o, $(OBJS)) $(TEST_OBJS) | $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) -o $@ $^ $(TEST_LDFLAGS)


# Compile main source files with dependency generation
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@


# Compile test files with dependency generation
$(BUILD_TEST_DIR)/%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(TEST_CFLAGS) -MMD -MP -c $< -o $@


# Include automatically generated dependencies
-include $(OBJS:.o=.d)
-include $(TEST_OBJS:.o=.d)


# Run Tests
test: $(TEST_EXEC)
	@echo "Running tests..."
	@$(TEST_EXEC)


# Run Program
run: $(EXEC)
	@./$(EXEC)


# Clean Build Artifacts
clean:
	rm -rf $(BUILD_DIR)


# Install Dependencies (Debian/Ubuntu)
deps:
	@echo "Installing dependencies..."
	@if command -v apt-get >/dev/null 2>&1; then \
		sudo apt-get install -y build-essential gcc make libcriterion-dev; \
	else \
		echo "Please install these packages manually: build-essential gcc make libcriterion-dev"; \
	fi


# Help Message
help:
	@echo "\nUsage:\n  make [target]\n\nTargets:\n  all       Build the main application (default)\n  test      Build and run tests\n  run       Build and run the main application\n  clean     Remove build artifacts\n  deps      Install required dependencies (Debian/Ubuntu)\n  help      Show this help message\n\nFlags:\n  DEBUG=0   Build optimized release version\n  -jN       Compile with parallel jobs (e.g., make -j4)\n"


.PHONY: all clean run test deps help
