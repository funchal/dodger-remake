.SUFFIXES:
.SECONDARY:

SHELL = /bin/sh

SFML = ../SFML

CXX = g++
CPPFLAGS = -Wall -Wextra -Wshadow -Wmissing-declarations -MMD -MP
CPPFLAGS += -I $(SFML)/include
CXXFLAGS = -g -fmessage-length=0 -O0

LD = g++
LDFLAGS = -rdynamic
LDLIBS = -L $(SFML)/lib -lsfml-window -lsfml-graphics -lsfml-system

TARGET = dodger-remake
OBJS = \
	main.o

.PHONY: check
check: all
	LD_LIBRARY_PATH=$(SFML)/lib ./$(TARGET)

.PHONY: all
all: $(TARGET)
	
$(TARGET): $(OBJS)
	$(LD) $^ -o $@ $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

%.o: %.cpp $(MAKEFILE_LIST)
	$(CXX) -c $< -o $@ $(CPPFLAGS) $(CXXFLAGS)
	
-include $(shell find . -name '*.d')