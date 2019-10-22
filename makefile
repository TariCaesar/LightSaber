CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy

# specify the device 
DEVICE := STM32F103xB

ifndef TARGET
TARGET := out
endif

# Define source file path
C_SRC_PATH := ./Src
CORE_SRC_PATH := ./Lib/Core/Src
LIB_SRC_PATH := ./Lib/StdPeriph/Src

# Define header file path
C_INC_PATH := ./Inc
CORE_INC_PATH := ./Lib/Core/Inc
LIB_INC_PATH := ./Lib/StdPeriph/Inc

#OBJ_PATH is used to save object file temporarily
OBJ_PATH := ./Obj

#BIN_PATH is used to save binary/hex file generated finally
BIN_PATH := ./Bin

#LINK_FILE specify the linker file used in ld
LINK_FILE := ./Lib/Core/STM32F103XB_FLASH.ld

CORE_C_SRC := $(wildcard $(CORE_SRC_PATH)/*.c)
CORE_ASM_SRC := $(wildcard $(CORE_SRC_PATH)/*.s)
LIB_C_SRC := $(wildcard $(LIB_SRC_PATH)/*.c)
LIB_ASM_SRC := $(wildcard $(LIB_SRC_PATH)/*.s)
C_SRC := $(wildcard $(C_SRC_PATH)/*.c)
ASM_SRC := $(wildcard $(C_SRC_PATH)/*.s)

CORE_C_OBJ := \
$(addprefix $(OBJ_PATH)/, $(patsubst %.c, %.o, $(notdir $(CORE_C_SRC))))
CORE_ASM_OBJ := \
$(addprefix $(OBJ_PATH)/, $(patsubst %.s, %.o, $(notdir $(CORE_ASM_SRC))))
LIB_C_OBJ := \
$(addprefix $(OBJ_PATH)/, $(patsubst %.c, %.o, $(notdir $(LIB_C_SRC))))
LIB_ASM_OBJ := \
$(addprefix $(OBJ_PATH)/, $(patsubst %.s, %.o, $(notdir $(LIB_ASM_SRC))))
C_OBJ = \
$(addprefix $(OBJ_PATH)/, $(patsubst %.c, %.o, $(notdir $(C_SRC))))
ASM_OBJ = \
$(addprefix $(OBJ_PATH)/, $(patsubst %.s, %.o, $(notdir $(ASM_SRC))))

###################### Compile Option ###########################
INC_FLAGS := -I $(C_INC_PATH) -I $(LIB_INC_PATH) -I $(CORE_INC_PATH)

C_FLAGS := -W -Wall -mcpu=cortex-m3 -mthumb --specs=nosys.specs \
-D $(DEVICE) -D USE_FULL_LL_DRIVER $(INC_FLAGS) 
C_DEBUG_FLAGS := -g 

ifndef mode
mode=debug
endif

ifeq ($(mode), debug)
C_FLAGS := $(C_FLAGS) $(C_DEBUG_FLAGS)
endif
#################################################################
.PHONY: all
all: $(C_OBJ) $(ASM_OBJ) $(CORE_C_OBJ) $(CORE_ASM_OBJ) $(LIB_C_OBJ) $(LIB_ASM_OBJ)
	$(CC) $(C_FLAGS) $(C_OBJ) $(ASM_OBJ) $(CORE_C_OBJ) $(CORE_ASM_OBJ) $(LIB_C_OBJ) $(LIB_ASM_OBJ) \
	-T $(LINK_FILE) -o $(BIN_PATH)/$(TARGET).elf 

	$(OBJCOPY) $(BIN_PATH)/$(TARGET).elf $(BIN_PATH)/$(TARGET).bin -Obinary
	$(OBJCOPY) $(BIN_PATH)/$(TARGET).elf $(BIN_PATH)/$(TARGET).hex -Oihex
#################################################################
$(C_OBJ): $(OBJ_PATH)/%.o: $(C_SRC_PATH)/%.c
	$(CC) $(C_FLAGS) -c $^ -o $@ 

$(ASM_OBJ): $(OBJ_PATH)/%.o: $(C_SRC_PATH)/%.s
	$(CC) $(C_FLAGS) -c $^ -o $@

$(CORE_C_OBJ): $(OBJ_PATH)/%.o: $(CORE_SRC_PATH)/%.c 
	$(CC) $(C_FLAGS) -c $^ -o $@

$(CORE_ASM_OBJ): $(OBJ_PATH)/%.o: $(CORE_SRC_PATH)/%.s
	$(CC) $(C_FLAGS) -c $^ -o $@

$(LIB_C_OBJ): $(OBJ_PATH)/%.o: $(LIB_SRC_PATH)/%.c
	$(CC) $(C_FLAGS) -c $^ -o $@ 

$(LIB_ASM_OBJ): $(OBJ_PATH)/%.o: $(LIB_SRC_PATH)/%.s
	$(CC) $(C_FLAGS) -c $^ -o $@

.PHONY: clean
clean:
	-rm $(C_OBJ)
	-rm $(ASM_OBJ)
	-rm $(CORE_C_OBJ)
	-rm $(CORE_ASM_OBJ)
	-rm $(LIB_C_OBJ)
	-rm $(LIB_ASM_OBJ)
	-rm $(BIN_PATH)/$(TARGET).elf
	-rm $(BIN_PATH)/$(TARGET).bin
	-rm $(BIN_PATH)/$(TARGET).hex

