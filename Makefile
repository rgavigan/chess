# Compiler and compiler flags
CXX = g++
CXXFLAGS = -g  # Add -g to the general compiler flags

INCLUDE_DIRS = $(addprefix -I , $(wildcard include/*)) -I include/ui/pages -I include/ui/components
BUILD = build

# Sources (.cpp) and objects (.o)
SOURCES = src/main.cpp $(wildcard src/*/*.cpp) $(wildcard src/*/*/*.cpp)
TEST_SOURCES = $(wildcard src/*/*.cpp) $(wildcard src/*/*/*.cpp) $(wildcard test/src/*/*.cpp)
OBJECTS = $(addprefix $(BUILD)/,$(SOURCES:.cpp=.o))
TEST_OBJECTS = $(addprefix $(BUILD)/,$(TEST_SOURCES:.cpp=.o))

# Libraries to link
LDFLAGS = -lsqlite3 -L$(WT_BASE)/lib -Wl,-rpath,$(WT_BASE)/lib -lwthttp -lwt
GTEST_FLAGS = -lgtest -lgtest_main

# Main application
all: $(BUILD) main_app

# Create build directory if it does not exist
$(BUILD):
	mkdir -p $(BUILD)/src/pieces $(BUILD)/src/game $(BUILD)/src/user $(BUILD)/src/ui $(BUILD)/src/ui/components $(BUILD)/src/ui/pages $(BUILD)/src/util $(BUILD)/test/src/pieces $(BUILD)/test/src/game $(BUILD)/test/src/user $(BUILD)/test/src/ui $(BUILD)/test/mocks/src/user

main_app: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Testing application
runtest: $(BUILD) test_main
	./test_main > $(BUILD)/test_console_output.txt 2>&1

test_main: $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(GTEST_FLAGS) -lsqlite3 > $(BUILD)/compiling_console_output.txt 2>&1
	valgrind --leak-check=full --show-leak-kinds=all ./test_main > $(BUILD)/valgrind_output.txt 2>&1

# Object creation in build directory
$(BUILD)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# Create doxygen documentation output
doc:
	doxygen doxygenConfig

# Remove executables and build directory
clean:
	rm -rf main_app test_main $(BUILD)/
