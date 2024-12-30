all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo "\tCC\t" $<
	@$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@echo "\tAS\t" $<
	@$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	@echo "\tLD\t" $<
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@echo "\tSIZE\t" $<
	@$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@echo "\tHEX\t" $<
	@$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	@echo "\tBIN\t" $<
	@$(BIN) $< $@

$(BUILD_DIR):
	@mkdir $@

.PHONY: clean
clean:
	@echo "\tRM\t" $(BUILD_DIR)
	@-rm -fR $(BUILD_DIR)

.PHONY: flash
flash: all
	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x8000000

.PHONY: flash-dfu
flash-dfu: all
	dfu-util -a 0 -D $(BUILD_DIR)/$(TARGET).bin -s 0x08000000

.PHONY: lint
lint:
	@echo "\tLINT\t"
	@find . -type f \( -name "*.c" -o -name "*.h" \) \
		\( -path "./src/*" \) \
		| xargs clang-format --Werror --style=file -i --dry-run

.PHONY: format
format:
	@echo "\tFORMAT\t"
	@find . -type f \( -name "*.c" -o -name "*.h" \) \
		\( -path "./src/*" \) \
		| xargs clang-format --Werror --style=file -i

