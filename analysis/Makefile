TARGET = analysis_transformer.o
SOURCES := $(wildcard src/*.cc) $(wildcard src/*/*.cc)
BUILD_DIR = build
LIB_DIR = libs
OBJECTS = $(SOURCES:.cc=.o)
BUILDOBJECTS := $(patsubst %,$(BUILD_DIR)/%,$(SOURCES:.cc=.o))
CFLAGS := -std=c++17 -g -O2 -Wall -Wextra -Werror -pedantic -I$(LIB_DIR)

.PHONY: all clean libs
all: $(TARGET)

%.o: %.cc
	mkdir -p $(BUILD_DIR)/$(dir $@)
	g++ $(CFLAGS) -o $(BUILD_DIR)/$@ -c $^

$(TARGET): $(OBJECTS)
	g++ $(CFLAGS) $(BUILDOBJECTS) -o $@ $(LDFLAGS)

clean:
	rm -r build || true
	rm $(TARGET) || true
