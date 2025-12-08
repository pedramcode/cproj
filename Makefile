# ============ INITIALIZE ============
CC := gcc
CC_FLAGS := -Wall -Werror -Wextra -g
ASM := nasm
ASM_FLAGS := -felf64
DEPFLAGS := -MMD -MP

SRC_DIR := ./src
INCLUDE_DIR := ./include
BUILD_DIR := ./build
EXE := main.out
TARGET := $(BUILD_DIR)/$(EXE)
LIBS := pthread crypto ssl
LIBS_FLAGS := $(addprefix -l, $(LIBS))
main: $(TARGET) # default command

# ============ BUILD ============
SOURCES_C := $(shell find $(SRC_DIR) -name "*.c")
SOURCES_ASM := $(shell find $(SRC_DIR) -name "*.asm")
OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES_C)) \
			$(patsubst $(SRC_DIR)/%.asm, $(BUILD_DIR)/%.o, $(SOURCES_ASM))
INCLUDE_FLAGS = $(addprefix -I, $(INCLUDE_DIR))

$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) $^ -o $@ $(LIBS_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) $(INCLUDE_FLAGS) $(DEPFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(dir $@)
	$(ASM) $(ASM_FLAGS) $< -o $@

# ============ PHONY ============
run: $(TARGET)
	$(TARGET) $(CONFIG)

rebuild: clean $(TARGET)

clean:
	@rm -rf $(BUILD_DIR)/*

debug: $(TARGET)
	gdb $(TARGET)

valgrind: $(TARGET)
	valgrind $(TARGET)

.PHONY: run clean debug rebuild valgrind

-include $(OBJECTS:.o=.d)