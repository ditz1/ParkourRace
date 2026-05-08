APP_NAME := parkourrace
GAME_DIR := game
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
LIB_DIR := lib

CXX := g++
CXXSTD := -std=c++20
WARN := -Wall -Wextra -pedantic
DEPSFLAGS := -MMD -MP
INCLUDES := -I$(GAME_DIR)/include -Iraylib

SRCS := \
	$(GAME_DIR)/main.cpp \
	$(GAME_DIR)/src/core/GameApp.cpp \
	$(GAME_DIR)/src/scene/GridWorld.cpp

OBJS := $(patsubst $(GAME_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

ifeq ($(OS),Windows_NT)
	TARGET := $(BUILD_DIR)/$(APP_NAME).exe
	LDFLAGS := -L$(LIB_DIR) -lraylib -lopengl32 -lgdi32 -lwinmm
	MKDIR_CMD = if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
	CLEAN_CMD = if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"
else
	TARGET := $(BUILD_DIR)/$(APP_NAME)
	MKDIR_CMD = mkdir -p "$(dir $@)"
	CLEAN_CMD = rm -rf "$(BUILD_DIR)"

	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		LDFLAGS := $(LIB_DIR)/libraylibmac.a -framework CoreVideo -framework Cocoa -framework IOKit -framework CoreFoundation -framework CoreGraphics -framework AudioToolbox
	else
		LDFLAGS := -L$(LIB_DIR) -lraylib -lGL -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -lXext -lpthread -ldl
	endif
endif

.PHONY: all clean run re

all: $(TARGET)

$(TARGET): $(OBJS)
	@$(MKDIR_CMD)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(GAME_DIR)/%.cpp
	@$(MKDIR_CMD)
	$(CXX) $(CXXSTD) $(WARN) $(INCLUDES) $(DEPSFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	$(CLEAN_CMD)

run: $(TARGET)
	$(TARGET)

re: clean all