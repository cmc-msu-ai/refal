CXX = g++
CXXFLAGS = -Wall -g
# -std=c++0x -O3

CPPFILES = main.cpp Refal.cpp Compiler.cpp Lang.cpp

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
DEPS_DIR = $(BUILD_DIR)/deps

# ------------------------------------------------------------------------------

CXXFLAGS += -I$(INCLUDE_DIR)

SOURCES = $(addprefix $(SRC_DIR)/, $(CPPFILES))
OBJECTS = $(addprefix $(OBJ_DIR)/, $(CPPFILES:.cpp=.o))
DEPS = $(addprefix $(DEPS_DIR)/, $(CPPFILES:.cpp=.d))

ifneq (clean, $(MAKECMDGOALS))
-include bridge.touch
endif

bridge.touch:
	mkdir -p $(BUILD_DIR) $(BIN_DIR) $(OBJ_DIR) $(DEPS_DIR)
	echo "-include $(DEPS_DIR)/deps.mk" > $@

all: $(BIN_DIR)/refal2

$(BIN_DIR)/refal2: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

# Pattern for generating dependency description files (*.d)
$(DEPS_DIR)/%.d: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -E -MM -MT \
		$(subst $(SRC_DIR)/, $(OBJ_DIR)/, $(<:.cpp=.o)) -MF $@ $<
		
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o \
		$(subst $(SRC_DIR)/, $(OBJ_DIR)/, $(<:.cpp=.o)) $<
	
$(DEPS_DIR)/deps.mk: $(DEPS)
	cat $^ > $(DEPS_DIR)/deps.mk

clean:
	rm -f bridge.touch
	rm -Rf build
