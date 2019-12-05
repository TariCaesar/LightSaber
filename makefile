CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy

# specify link file according to device 
ifeq ($(DEVICE), STM32F103xE)
LINK_FILE := ./Lib/Core/STM32F1XX_HD.ld
else ifeq ($(DEVICE), STM32F103xB)
LINK_FILE := ./Lib/Core/STM32F1XX_MD.ld
else
$(error 'DEVICE' should be 'STM32F103xE' or 'STM32F103xB')
endif

ifndef TARGET
TARGET := out
endif

C_DEBUG_FLAGS := -ggdb3 -gdwarf-4
C_RELEASE_FLAGS := -Os

# specify debug or release mode
ifndef MODE
MODE := DEBUG
endif

#BIN_PATH is used to save binary/hex file generated finally
ifeq ($(MODE), DEBUG)
ifdef C_FLAGS
C_FLAGS += $(C_DEBUG_FLAGS)
else
C_FLAGS := $(C_DEBUG_FLAGS)
endif
BIN_PATH := ./Debug
else ifeq ($(MODE), RELEASE)
ifdef C_FLAGS
C_FLAGS += $(C_RELEASE_FLAGS)
else
C_FLAGS := $(C_RELEASE_FLAGS)
endif
BIN_PATH := ./Release
else
$(error 'MODE' should be 'DEBUG' or 'RELEASE')
endif

#OBJ_PATH is used to save object file temporarily
OBJ_PATH := $(BIN_PATH)/Obj

#Check if the path exist 
#If not, create it
ifeq (, $(wildcard $(BIN_PATH)/.*))
$(shell	mkdir "$(BIN_PATH)")
endif
ifeq (, $(wildcard $(OBJ_PATH)/.*))
$(shell	mkdir "$(OBJ_PATH)")
endif

# Define source file path
MY_SRC_PATH := ./Src
CORE_SRC_PATH := ./Lib/Core/Src
LIB_SRC_PATH := ./Lib/StdPeriph/Src

# Define header file path
MY_INC_PATH := ./Inc
CORE_INC_PATH := ./Lib/Core/Inc
LIB_INC_PATH := ./Lib/StdPeriph/Inc

CORE_C_SRC := $(wildcard $(CORE_SRC_PATH)/*.c)
LIB_C_SRC := $(wildcard $(LIB_SRC_PATH)/*.c)
LIB_ASM_SRC := $(wildcard $(LIB_SRC_PATH)/*.s)
MY_C_SRC := $(wildcard $(MY_SRC_PATH)/*.c)
MY_ASM_SRC := $(wildcard $(MY_SRC_PATH)/*.s)

ifeq ($(DEVICE), STM32F103xE)
CORE_ASM_SRC := $(CORE_SRC_PATH)/startup_stm32f103xe.s
else ifeq ($(DEVICE), STM32F103xB)
CORE_ASM_SRC := $(CORE_SRC_PATH)/startup_stm32f103xb.s
endif

CORE_C_OBJ := \
$(addprefix $(OBJ_PATH)/, $(patsubst %.c, %.o, $(notdir $(CORE_C_SRC))))
CORE_ASM_OBJ := \
$(addprefix $(OBJ_PATH)/, $(patsubst %.s, %.o, $(notdir $(CORE_ASM_SRC))))
LIB_C_OBJ := \
$(addprefix $(OBJ_PATH)/, $(patsubst %.c, %.o, $(notdir $(LIB_C_SRC))))
LIB_ASM_OBJ := \
$(addprefix $(OBJ_PATH)/, $(patsubst %.s, %.o, $(notdir $(LIB_ASM_SRC))))
MY_C_OBJ := \
$(addprefix $(OBJ_PATH)/, $(patsubst %.c, %.o, $(notdir $(MY_C_SRC))))
MY_ASM_OBJ := \
$(addprefix $(OBJ_PATH)/, $(patsubst %.s, %.o, $(notdir $(MY_ASM_SRC))))

INC_FLAGS := -I $(MY_INC_PATH) -I $(LIB_INC_PATH) -I $(CORE_INC_PATH)

INC_FILES := \
$(wildcard $(MY_INC_PATH)/*.h) \
$(wildcard $(LIB_INC_PATH)/*.h) \
$(wildcard $(CORE_INC_PATH)/*.h) 

C_FLAGS += -std=c99 -W -Wall -mcpu=cortex-m3 -mthumb --specs=nosys.specs -ffunction-sections \
-fdata-sections -Wl,--gc-sections -D $(DEVICE) -D USE_FULL_LL_DRIVER $(INC_FLAGS) 


########################### make #################################
.PHONY: all
all: $(MY_C_OBJ) $(MY_ASM_OBJ) $(CORE_C_OBJ) $(CORE_ASM_OBJ) $(LIB_C_OBJ) $(LIB_ASM_OBJ)

	$(CC) $(C_FLAGS) $(MY_C_OBJ) $(MY_ASM_OBJ) $(CORE_C_OBJ) $(CORE_ASM_OBJ) $(LIB_C_OBJ) \
	$(LIB_ASM_OBJ) -T $(LINK_FILE) -o $(BIN_PATH)/$(TARGET).elf 

	$(OBJCOPY) $(BIN_PATH)/$(TARGET).elf $(BIN_PATH)/$(TARGET).bin -Obinary
	$(OBJCOPY) $(BIN_PATH)/$(TARGET).elf $(BIN_PATH)/$(TARGET).hex -Oihex
#################################################################
$(MY_C_OBJ): $(OBJ_PATH)/%.o: $(MY_SRC_PATH)/%.c $(INC_FILES)
	$(CC) $(C_FLAGS) -c $< -o $@ 

$(MY_ASM_OBJ): $(OBJ_PATH)/%.o: $(MY_SRC_PATH)/%.s $(INC_FILES)
	$(CC) $(C_FLAGS) -c $< -o $@

$(CORE_C_OBJ): $(OBJ_PATH)/%.o: $(CORE_SRC_PATH)/%.c $(INC_FILES)
	$(CC) $(C_FLAGS) -c $< -o $@

$(CORE_ASM_OBJ): $(OBJ_PATH)/%.o: $(CORE_SRC_PATH)/%.s $(INC_FILES)
	$(CC) $(C_FLAGS) -c $< -o $@

$(LIB_C_OBJ): $(OBJ_PATH)/%.o: $(LIB_SRC_PATH)/%.c $(INC_FILES)
	$(CC) $(C_FLAGS) -c $< -o $@ 

$(LIB_ASM_OBJ): $(OBJ_PATH)/%.o: $(LIB_SRC_PATH)/%.s $(INC_FILES)
	$(CC) $(C_FLAGS) -c $< -o $@
