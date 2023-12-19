CXXFLAGS := -Iinclude -D_DEFAULT_SOURCE -std=gnu++20 -Wall -Wextra -Werror
LDFLAGS :=

sources := $(wildcard src/*.cpp)
headers := $(wildcard src/*.hpp)
lib_headers := $(wildcard include/turbine/**/*.hpp)

all: turbine

clean:
	$(RM) include/turbine.hpp turbine

turbine: include/turbine.hpp $(sources) $(headers)
	$(CXX) $(strip $(CXXFLAGS) -o $@ $(sources) $(LDFLAGS))

include/turbine.hpp: scripts/amalgamate.py $(lib_headers)
	scripts/amalgamate.py > $@

.PHONY: all clean
