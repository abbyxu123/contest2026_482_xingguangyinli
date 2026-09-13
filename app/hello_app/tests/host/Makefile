CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -pedantic -O2

TARGET := test_lc_state
SOURCES := test_lc_state.c ../../src/lc_state.c
INCLUDES := -I../../include

.PHONY: all test clean

all: $(TARGET)

$(TARGET): $(SOURCES) ../../include/lc_state.h
	$(CC) $(CFLAGS) $(INCLUDES) $(SOURCES) -o $@

test: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(TARGET) $(TARGET).dSYM
