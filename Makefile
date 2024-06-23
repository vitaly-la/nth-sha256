CXX = g++
CXXFLAGS = -std=c++20 -O3 -fomit-frame-pointer -DTEST_MAIN -msse4.1 -mavx512vl -mavx512vbmi -msha -Wall -Wextra -Weffc++ -Werror
LDLIBS =

PRGM  = test
SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean

all: $(PRGM)

$(PRGM): $(OBJS)
	$(CXX) $(OBJS) $(LDLIBS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(OBJS) $(DEPS) $(PRGM)

-include $(DEPS)
