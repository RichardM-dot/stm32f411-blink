###############################################################################
# Makefile for stm32f411-blink
#
# Bare-metal STM32F411xE firmware built with the local ARM GNU toolchain
# (no global PATH setup required - everything points at TOOLCHAIN_DIR).
#
# Targets:
#   make          - build ELF + HEX + BIN and print a size summary
#   make dis      - generate build/<target>.dis (full disassembly)
#   make symbols  - generate build/<target>.sym (sorted symbol table)
#   make ccdb     - generate compile_commands.json (editor code navigation)
#   make clean    - remove all build artifacts
#
###############################################################################

######################################
# Target / toolchain
######################################
.DEFAULT_GOAL := all
TARGET       = stm32f411_blink
BUILD_DIR    = build

# Local ARM GNU toolchain (Arm GNU Toolchain 15.3.Rel2, darwin-arm64 host)
TOOLCHAIN_DIR = /Users/theo-richard/workspace/toolchains/arm-none-eabi
CC      = $(TOOLCHAIN_DIR)/bin/arm-none-eabi-gcc
AS      = $(TOOLCHAIN_DIR)/bin/arm-none-eabi-gcc -x assembler-with-cpp
CP      = $(TOOLCHAIN_DIR)/bin/arm-none-eabi-objcopy
OD      = $(TOOLCHAIN_DIR)/bin/arm-none-eabi-objdump
NM      = $(TOOLCHAIN_DIR)/bin/arm-none-eabi-nm
SIZE    = $(TOOLCHAIN_DIR)/bin/arm-none-eabi-size

######################################
# Sources
######################################
# Application
C_SOURCES = \
Core/Src/main.c \
Core/Src/stm32f4xx_it.c \
Core/Src/stm32f4xx_hal_msp.c \
Core/Src/system_stm32f4xx.c

# HAL drivers used by this application
# (keep in sync with Core/Inc/stm32f4xx_hal_conf.h)
C_SOURCES += \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c

# FreeRTOS kernel V11.1.0 (GCC / ARM_CM4F port, heap_4)
# Kernel config and HAL timebase glue live in Config/
C_SOURCES += \
Middlewares/Third_Party/FreeRTOS/Source/tasks.c \
Middlewares/Third_Party/FreeRTOS/Source/queue.c \
Middlewares/Third_Party/FreeRTOS/Source/list.c \
Middlewares/Third_Party/FreeRTOS/Source/timers.c \
Middlewares/Third_Party/FreeRTOS/Source/event_groups.c \
Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c \
Middlewares/Third_Party/FreeRTOS/Source/croutine.c \
Middlewares/Third_Party/FreeRTOS/portable/GCC/ARM_CM4F/port.c \
Middlewares/Third_Party/FreeRTOS/portable/MemMang/heap_4.c \
Config/stm32f4xx_hal_timebase.c

# ST startup code (vector table, Reset_Handler, .data/.bss init)
ASM_SOURCES = \
startup_stm32f411xe.s

######################################
# C includes
######################################
C_INCLUDES = \
-ICore/Inc \
-IConfig \
-IDrivers/CMSIS/Device/ST/STM32F4xx/Include \
-IDrivers/CMSIS/Include \
-IDrivers/STM32F4xx_HAL_Driver/Inc \
-IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy \
-IMiddlewares/Third_Party/FreeRTOS/Source/include \
-IMiddlewares/Third_Party/FreeRTOS/portable/GCC/ARM_CM4F \
-IMiddlewares/Third_Party/FreeRTOS/portable/MemMang

######################################
# Compiler / linker flags
######################################
# Cortex-M4F: hard-float ABI with FPv4-SP single-precision FPU
CPU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# Preprocessor defines
C_DEFS = -DUSE_HAL_DRIVER -DSTM32F411xE

CFLAGS  = $(CPU) $(C_DEFS) $(C_INCLUDES) -Wall -Wextra
CFLAGS += -fdiagnostics-color=always -fdata-sections -ffunction-sections
CFLAGS += -Og -g3
ASFLAGS = $(CPU) $(C_DEFS) $(C_INCLUDES) -Wall -fdata-sections -ffunction-sections

# Link with newlib-nano, no semihosting, garbage-collect unused sections
LDLIBS  = -lc -lm -lnosys
LDFLAGS = $(CPU) -specs=nano.specs -specs=nosys.specs -T STM32F411xE_FLASH.ld
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--print-memory-usage

######################################
# Build rules
######################################
# Intermediate objects/dep/listings go to build/obj/, so the top of build/
# only ever contains the final firmware artifacts.
OBJ_DIR = $(BUILD_DIR)/obj

OBJECTS = $(addprefix $(OBJ_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

OBJECTS += $(addprefix $(OBJ_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(OBJ_DIR)/%.o: %.c Makefile | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -Wa,-a,-ad,-alms=$(@:%.o=%.lst) $< -o $@

$(OBJ_DIR)/%.o: %.s Makefile | $(OBJ_DIR)
	$(AS) -c $(ASFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/$(TARGET).elf | $(BUILD_DIR)
	$(CP) -O ihex $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/$(TARGET).elf | $(BUILD_DIR)
	$(CP) -O binary -S $< $@

$(BUILD_DIR)/%.dis: $(BUILD_DIR)/$(TARGET).elf
	$(OD) -d -S --disassemble-zeroes $< > $@

$(BUILD_DIR)/%.sym: $(BUILD_DIR)/$(TARGET).elf
	$(NM) -n -S --size-sort $< > $@

$(BUILD_DIR) $(OBJ_DIR):
	mkdir -p $@

elf: $(BUILD_DIR)/$(TARGET).elf
bin: $(BUILD_DIR)/$(TARGET).bin
hex: $(BUILD_DIR)/$(TARGET).hex
dis: $(BUILD_DIR)/$(TARGET).dis
symbols: $(BUILD_DIR)/$(TARGET).sym

# compile_commands.json: lets clangd / VS Code / CLion resolve every include
# and "go to definition" into HAL, CMSIS and FreeRTOS sources inside this
# project. Re-run after adding/removing source files.
ccdb:
	@$(MAKE) -nB $(OBJECTS) 2>/dev/null | awk -v dir="$(CURDIR)" 'BEGIN{printf "["} \
	/-none-eabi-gcc/ && /[ ]-c[ ]/ { \
	  gsub(/"/, ""); \
	  n = split($$0, w, " "); \
	  src = ""; \
	  for (i = 1; i < n; i++) if (w[i] == "-o") src = w[i-1]; \
	  if (src != "") { \
	    if (cnt++ > 0) printf ",\n"; \
	    printf "{\"directory\": \"%s\", \"command\": \"%s\", \"file\": \"%s\"}", dir, $$0, src; \
	  } \
	} \
	END { printf "\n]\n" }' > compile_commands.json
	@echo "compile_commands.json: $$(grep -c '"file"' compile_commands.json) entries"

size: $(BUILD_DIR)/$(TARGET).elf
	$(SIZE) $(BUILD_DIR)/$(TARGET).elf

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin size

######################################
# Housekeeping
######################################
clean:
	-rm -fR $(BUILD_DIR)

-include $(wildcard $(OBJ_DIR)/*.d)

.PHONY: all elf bin hex dis symbols ccdb size clean
