CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -pedantic -O2

TARGETS := test_lc_state test_lc_clock test_lc_dinner test_lc_memory test_lc_ui
INCLUDES := -I../../include

.PHONY: all test clean

all: $(TARGETS)

test_lc_state: test_lc_state.c ../../src/lc_state.c ../../include/lc_state.h
	$(CC) $(CFLAGS) $(INCLUDES) test_lc_state.c ../../src/lc_state.c -o $@

test_lc_clock: test_lc_clock.c ../../src/lc_clock.c ../../include/lc_clock.h
	$(CC) $(CFLAGS) $(INCLUDES) test_lc_clock.c ../../src/lc_clock.c -o $@

test_lc_dinner: test_lc_dinner.c ../../src/lc_dinner.c ../../include/lc_dinner.h ../../include/lc_state.h
	$(CC) $(CFLAGS) $(INCLUDES) test_lc_dinner.c ../../src/lc_dinner.c -o $@

test_lc_memory: test_lc_memory.c ../../src/lc_memory.c ../../include/lc_memory.h ../../include/lc_dinner.h
	$(CC) $(CFLAGS) $(INCLUDES) test_lc_memory.c ../../src/lc_memory.c -o $@

test_lc_ui: test_lc_ui.c ../../src/lc_ui.c ../../src/lc_clock.c ../../include/lc_ui.h
	$(CC) $(CFLAGS) $(INCLUDES) test_lc_ui.c ../../src/lc_ui.c ../../src/lc_clock.c -o $@

test: $(TARGETS)
	./test_lc_state
	./test_lc_clock
	./test_lc_dinner
	./test_lc_memory
	./test_lc_ui

clean:
	rm -rf $(TARGETS) $(addsuffix .dSYM,$(TARGETS))
