CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -pedantic -O2

TARGETS := test_lc_state test_lc_clock
INCLUDES := -I../../include

.PHONY: all test clean

all: $(TARGETS)

test_lc_state: test_lc_state.c ../../src/lc_state.c ../../include/lc_state.h
	$(CC) $(CFLAGS) $(INCLUDES) test_lc_state.c ../../src/lc_state.c -o $@

test_lc_clock: test_lc_clock.c ../../src/lc_clock.c ../../include/lc_clock.h
	$(CC) $(CFLAGS) $(INCLUDES) test_lc_clock.c ../../src/lc_clock.c -o $@

test: $(TARGETS)
	./test_lc_state
	./test_lc_clock

clean:
	rm -rf $(TARGETS) $(addsuffix .dSYM,$(TARGETS))
