# ============================================================================
# Project Configuration
# ============================================================================

# Project name
PROJECT_NAME = optimizer

# Directories
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build
BUILD_OBJ_DIR = $(BUILD_DIR)/obj
BUILD_TEST_DIR = $(BUILD_DIR)/tests
LIB_DIR = lib
INCLUDE_DIR = include
WEB_DIR = web

# Executables
EXEC = $(BUILD_DIR)/$(PROJECT_NAME)
TEST_EXEC = $(BUILD_TEST_DIR)/run_tests

# ============================================================================
# Source and Object Files
# ============================================================================

# Recursively find all source files
SRCS = $(shell find $(SRC_DIR) -name '*.c')
TEST_SRCS = $(shell find $(TEST_DIR) -name '*.c')

# Generate object file paths
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_OBJ_DIR)/%.o,$(SRCS))
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c,$(BUILD_TEST_DIR)/%.o,$(TEST_SRCS))

# Dependencies
DEPS = $(OBJS:.o=.d) $(TEST_OBJS:.o=.d)

# ============================================================================
# Compiler Configuration
# ============================================================================

# Tools
CC = gcc
AR = ar
MKDIR = mkdir -p
RM = rm -rf

# Base compiler flags
WARNINGS = -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wdouble-promotion
CFLAGS = -std=c2x $(WARNINGS)
CFLAGS += $(INCLUDE_PATHS)

# Architecture optimizations
CFLAGS += -march=native -mtune=native

# Linker flags
LDFLAGS = -flto -Wl,-O2,--as-needed $(SCIP_LIBS)

# External libraries
SCIP_CFLAGS = -I/usr/include/scip
SCIP_LIBS = -lscip -lsoplex -lreadline -lncurses -lm -lz -lgmp -lstdc++

# Include paths
INCLUDE_PATHS = -I$(INCLUDE_DIR) -I$(SRC_DIR) $(SCIP_CFLAGS)

# Test framework
TEST_CFLAGS = $(shell pkg-config --cflags criterion) -pthread -DCRITERION_ENABLE_DEBUG
TEST_LDFLAGS = $(shell pkg-config --libs criterion) -Wl,--no-as-needed -pthread

# ============================================================================
# Build Configuration
# ============================================================================

# Debug mode (0=release, 1=debug, 2=profile)
DEBUG ?= 0

# Debug build with symbols
ifeq ($(DEBUG),1)
    CFLAGS += -g -DDEBUG -O0
    BUILD_TYPE = debug

# Debug build with profiling
else ifeq ($(DEBUG),2)
    CFLAGS += -g -DPROFILE -pg -O0
    LDFLAGS += -pg
    BUILD_TYPE = profile

# Release build
else
    CFLAGS += -O2 -DNDEBUG -fvisibility=hidden
    LDFLAGS += -s
    BUILD_TYPE = release
endif

# Add SCIP flags
CFLAGS += $(SCIP_CFLAGS)
LDFLAGS += $(SCIP_LIBS)

# ============================================================================
# Build Rules
# ============================================================================

# Default target
.PHONY: all
all: $(EXEC)



# Link the application
$(EXEC): $(OBJS)
	@echo "[$(BUILD_TYPE)] Linking $@"
	@$(MKDIR) $(@D)
	@$(CC) $(OBJS) $(LDFLAGS) -o $@ -pthread

# Test executable
$(TEST_EXEC): $(filter-out $(BUILD_OBJ_DIR)/main.o, $(OBJS)) $(TEST_OBJS) | $(BUILD_DIR)
	@echo "[$(BUILD_TYPE)] Linking $@"
	@$(CC) -o $@ $^ $(LDFLAGS) $(TEST_LDFLAGS) -pthread



# Compile source files
$(BUILD_OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_OBJ_DIR)
	@echo "[$(BUILD_TYPE)] Compiling $<"
	@$(MKDIR) $(@D)
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Compile test files
$(BUILD_TEST_DIR)/%.o: $(TEST_DIR)/%.c | $(BUILD_TEST_DIR)
	@echo "[$(BUILD_TYPE)] Compiling test $<"
	@$(MKDIR) $(@D)
	@$(CC) $(CFLAGS) $(TEST_CFLAGS) -I$(TEST_DIR) -c $< -o $@

# Create build directories
$(BUILD_DIR) $(BUILD_OBJ_DIR) $(BUILD_TEST_DIR):
	@$(MKDIR) $@

# Include generated dependencies
-include $(DEPS)

# ============================================================================
# Phony Targets
# ============================================================================

.PHONY: run test clean distclean deps help

# Run the application
run: $(EXEC)
	@echo "[$(BUILD_TYPE)] Running $(EXEC)"
	@./$(EXEC)

# Run tests
test: $(TEST_EXEC)
	@echo "[$(BUILD_TYPE)] Running tests"
	@cd $(BUILD_TEST_DIR) && LD_LIBRARY_PATH=/usr/local/lib ./run_tests --verbose=2 --full-stats || true

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts"
	@$(RM) $(BUILD_DIR)
	@find $(LIB_DIR) -name '*.o' -type f -delete

# Clean everything including downloaded dependencies
distclean: clean
	@echo "Cleaning everything"
	@$(RM) $(LIB_DIR)/*.a $(LIB_DIR)/*.o

# Install dependencies (Debian/Ubuntu)
deps:
	@echo "Installing dependencies..."
	@if command -v apt-get >/dev/null 2>&1; then \
		echo "[apt] Installing build tools and libraries"; \
		sudo apt-get install -y build-essential gcc make pkg-config \
		    libscip-dev libsoplex-dev libreadline-dev libncurses5-dev \
		    zlib1g-dev libgmp-dev libcriterion-dev; \
	else \
		echo "Please install these packages manually:"; \
		echo "  build-essential gcc make pkg-config"; \
		echo "  libscip-dev libsoplex-dev libreadline-dev libncurses5-dev"; \
		echo "  zlib1g-dev libgmp-dev libcriterion-dev"; \
	fi

# Show help message
help:
	@echo "\n$(PROJECT_NAME) - Build System\n"
	@echo "Usage:"
	@echo "  make [target] [VARIABLE=value]\n"
	@echo "Build targets:"
	@echo "  all       Build the main application (default)"
	@echo "  test      Build and run tests"
	@echo "  run       Build and run the main application"
	@echo "  clean     Remove build artifacts"
	@echo "  distclean Remove all generated files"
	@echo "  deps      Install required dependencies"
	@echo "  help      Show this help message\n"
	@echo "Build options:"
	@echo "  DEBUG=0   Build type: 0=release (default), 1=debug, 2=profile"
	@echo "  -jN       Compile with N parallel jobs (e.g., make -j4)"
	@echo "  V=1       Enable verbose build output\n"
	@echo "Current configuration:"
	@echo "  CC        = $(CC)"
	@echo "  CFLAGS    = $(CFLAGS)"
	@echo "  LDFLAGS   = $(LDFLAGS)"
	@echo "  BUILD_DIR = $(BUILD_DIR)"
	@echo "  BUILD_TYPE= $(BUILD_TYPE)\n"
