PROJECT_NAME=Atmega4809_SD_FAT32
MCU=atmega4809
F_CPU?=16000000UL

CC=avr-gcc
OBJCOPY=avr-objcopy
SIZE=avr-size

SRC_DIR=src
BUILD_DIR=build

SRCS=$(wildcard $(SRC_DIR)/*.c)
OBJS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

CFLAGS=-mmcu=$(MCU) -Os -std=c11 -Wall -Wextra -Werror -DF_CPU=$(F_CPU)
LDFLAGS=-mmcu=$(MCU)

all: dirs $(BUILD_DIR)/$(PROJECT_NAME).elf $(BUILD_DIR)/$(PROJECT_NAME).hex

dirs:
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(PROJECT_NAME).elf: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@
	$(SIZE) -C --mcu=$(MCU) $@

$(BUILD_DIR)/$(PROJECT_NAME).hex: $(BUILD_DIR)/$(PROJECT_NAME).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

flash: $(BUILD_DIR)/$(PROJECT_NAME).hex
	@echo "Use your preferred programmer (e.g., atmel-ice) to flash the HEX."

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all dirs flash clean
