CXXFLAGS := -Iinclude -D_DEFAULT_SOURCE -std=gnu++20 -Wall -Wextra -Werror
LDFLAGS :=

sources := $(wildcard src/*.cpp) $(wildcard lib/*.cpp)
headers := \
	$(wildcard src/*.hpp) \
	$(wildcard lib/*.hpp) \
	$(wildcard include/turbine/*.hpp) \
	$(wildcard include/turbine/common/*.hpp) \
	$(wildcard include/turbine/io/*.hpp) \
	$(wildcard include/turbine/linux/*.hpp) \
	$(wildcard include/turbine/net/*.hpp) \
	$(wildcard include/turbine/net/tcp/*.hpp) \
	$(wildcard include/turbine/posix/*.hpp)

all: turbine

clean:
	$(RM) turbine

turbine: $(sources) $(headers)
	$(CXX) $(strip $(CXXFLAGS) -o $@ $(sources) $(LDFLAGS))

.PHONY: all clean
