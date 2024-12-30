TARGET = DrunkBeeper
DEBUG = 1
OPT = -Og
BUILD_DIR = build

C_SOURCES += $(shell find src -type f -name "*.c")

C_SOURCES +=  \
	stm/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_gpio.c \
	stm/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_rcc.c \
	stm/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_utils.c \
	stm/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_exti.c \
	stm/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_tim.c \
	stm/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_dma.c \
	stm/Core/Src/system_stm32f4xx.c

ASM_SOURCES =  \
	stm/startup_stm32f401xe.s

PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size

HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

CPU = -mcpu=cortex-m4

FPU = -mfpu=fpv4-sp-d16

FLOAT-ABI = -mfloat-abi=hard

MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

AS_DEFS =

C_DEFS =  \
	-DUSE_FULL_LL_DRIVER \
	-DSTM32F401xE \
	-DHSE_VALUE=25000000 \
	-DHSE_STARTUP_TIMEOUT=100 \
	-DLSE_STARTUP_TIMEOUT=5000 \
	-DLSE_VALUE=32768 \
	-DEXTERNAL_CLOCK_VALUE=12288000 \
	-DHSI_VALUE=16000000 \
	-DLSI_VALUE=32000 \
	-DVDD_VALUE=3300 \
	-DPREFETCH_ENABLE=1 \
	-DINSTRUCTION_CACHE_ENABLE=1 \
	-DDATA_CACHE_ENABLE=1

AS_INCLUDES =

C_INCLUDES =  \
	-Isrc \
	-Istm/Core/Inc \
	-Istm/Drivers/STM32F4xx_HAL_Driver/Inc \
	-Istm/Drivers/CMSIS/Device/ST/STM32F4xx/Include \
	-Istm/Drivers/CMSIS/Include

ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -Werror

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

LDSCRIPT = STM32F401CDUx_FLASH.ld

LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

