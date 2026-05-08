APP_NAME := animtester
SRC := main.cpp
BUILD_DIR := build
TARGET := $(BUILD_DIR)/$(APP_NAME)

CXX := g++
CXXSTD := -std=c++20
WARN := -Wall -Wextra -pedantic
INCLUDES := -I../raylib

ifeq ($(OS),Windows_NT)
	TARGET := $(BUILD_DIR)/$(APP_NAME).exe
	LDFLAGS := -L../lib -lraylib -lopengl32 -lgdi32 -lwinmm
	MKDIR_CMD = if not exist "$(subst /,\,$(BUILD_DIR))" mkdir "$(subst /,\,$(BUILD_DIR))"
	CLEAN_CMD = if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"
	RUN_CMD = .\$(TARGET)
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		LDFLAGS := ../lib/libraylibmac.a -framework CoreVideo -framework Cocoa -framework IOKit -framework CoreFoundation -framework CoreGraphics -framework AudioToolbox
	else
		LDFLAGS := -L../lib -lraylib -lGL -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -lXext -lpthread -ldl
	endif
	MKDIR_CMD = mkdir -p "$(BUILD_DIR)"
	CLEAN_CMD = rm -rf "$(BUILD_DIR)"
	RUN_CMD = ./$(TARGET)
endif

.PHONY: all clean run re

all: $(TARGET)

$(TARGET): $(SRC)
	@$(MKDIR_CMD)
	$(CXX) $(CXXSTD) $(WARN) $(INCLUDES) $(SRC) -o $@ $(LDFLAGS)

clean:
	$(CLEAN_CMD)

run: $(TARGET)
	$(RUN_CMD)

re: clean all
