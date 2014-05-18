.SUFFIXES:
.SECONDARY:

SHELL = /bin/sh

SFML = ../SFML

CXX = g++
CPPFLAGS = -Wall -Wextra -Wshadow -Wmissing-declarations -MMD -MP
CPPFLAGS += -I $(SFML)/include
CXXFLAGS = -g -fmessage-length=0 -O0

LD = g++
LDFLAGS = -rdynamic --enable-new-dtags -Wl,-rpath $(SFML)/lib
LDFLAGS += -L $(SFML)/lib -lsfml-window -lsfml-graphics -lsfml-system

TARGET = Dodger
OBJS = \
	Dodger.o

.PHONY: check
check: all
	./$(TARGET)

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -f $(shell find . -name '*.o')
	rm -f $(shell find . -name '*.d')

$(TARGET): $(OBJS)
	$(LD) $^ -o $@ $(CXXFLAGS) $(LDFLAGS)

%.o: %.cpp $(MAKEFILE_LIST)
	$(CXX) -c $< -o $@ $(CPPFLAGS) $(CXXFLAGS)

-include $(shell find . -name '*.d')
