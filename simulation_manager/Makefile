TARGET = simulation_manager.o
SOURCES := $(wildcard src/*.cc) $(wildcard src/*/*.cc)
BUILD_DIR = build
LIB_DIR = libs
OBJECTS = $(SOURCES:.cc=.o)
BUILDOBJECTS := $(patsubst %,$(BUILD_DIR)/%,$(SOURCES:.cc=.o))
CFLAGS := -std=c++11 -O2 -Wall -Wextra -Werror -pedantic -Ilibs
CFLAGS += -I$(SUMO_HOME)/src
LDFLAGS := -L$(SUMO_HOME)/bin -lsumocpp -ltracicpp

.PHONY: all clean
all: $(TARGET)

libs:
	wget https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
	mkdir -p $(LIB_DIR) && mv json.hpp $(LIB_DIR)

%.o: %.cc
	mkdir -p $(BUILD_DIR)/$(dir $@)
	g++ $(CFLAGS) -o $(BUILD_DIR)/$@ -c $^

$(TARGET): $(OBJECTS)
	g++ $(CFLAGS) $(BUILDOBJECTS) -o $@ $(LDFLAGS)

clean:
	rm -r build || true
	rm $(TARGET) || true
