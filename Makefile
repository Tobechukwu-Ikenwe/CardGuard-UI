# CardGuard Makefile
# Builds CLI validator. Web UI: open web/index.html in a browser.

CXX ?= g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
TARGET = card_validator

SRC = src/main.cpp src/validator.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC)

clean:
	rm -f $(TARGET)
	@if exist $(TARGET).exe del $(TARGET).exe 2>nul || true

.PHONY: all clean
