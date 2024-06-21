CC = gcc
CFLAGS = -std=c99 -O3 -fomit-frame-pointer -DTEST_MAIN -msse4.1 -mavx512vl -mavx512vbmi -msha -Wall -Wextra -Werror
LDLIBS =

PRGM  = test
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean

all: $(PRGM)

$(PRGM): $(OBJS)
	$(CC) $(OBJS) $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(OBJS) $(DEPS) $(PRGM)

-include $(DEPS)
