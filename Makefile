SHELL := /bin/bash

# Specify the files to compile and the name of the final binary
SRCS=zimage.c
BIN=zimage.bin
LENA=lena/lena_4.bmp

# Directory where source files are and where the binaries will be put
INPUT_DIR=src
INCLUDE_DIR=include
OUTPUT_DIR=bin

# Include directory containing Zeal 8-bit OS header files.
ifndef ZOS_PATH
$(error "Please define ZOS_PATH environment variable. It must point to Zeal 8-bit OS source code path.")
endif

ifndef ZFS_PATH
$(error "Please define ZFS_PATH environment variable. It must point to ZealFS source code path.")
endif

ZOS_INCLUDE=$(ZOS_PATH)/kernel_headers/sdcc/include/
# Regarding the linking process, we will need to specify the path to the crt0 REL file.
# It contains the boot code for C programs as well as all the C functions performing syscalls.
CRT_REL=$(ZOS_PATH)/kernel_headers/sdcc/bin/zos_crt0.rel


# Compiler, linker and flags related variables
CC=sdcc
# Specify Z80 as the target, compile without linking, and place all the code in TEXT section
# (_CODE must be replace).
CFLAGS=-mz80 -c --codeseg TEXT -I$(ZOS_INCLUDE) -I$(INCLUDE_DIR)
LD=sdldz80
# Make sure the whole program is relocated at 0x4000 as request by Zeal 8-bit OS.
LDFLAGS=-n -mjwx -i -b _HEADER=0x4000 $(SDLD_FLAGS) -k $(ZOS_PATH)/kernel_headers/sdcc/lib -l z80
# Binary used to convert ihex to binary
OBJCOPY=objcopy

# Generate the intermediate Intel Hex binary name
BIN_HEX=$(patsubst %.bin,%.ihx,$(BIN))
# Generate the rel names for C source files. Only keep the file names, and add output dir prefix.
SRCS_OUT_DIR=$(addprefix $(OUTPUT_DIR)/,$(SRCS))
SRCS_REL=$(patsubst %.c,%.rel,$(SRCS_OUT_DIR))


.PHONY: all clean

all: clean $(OUTPUT_DIR) $(OUTPUT_DIR)/$(BIN_HEX) $(OUTPUT_DIR)/$(BIN)
	cp $(LENA) $(OUTPUT_DIR)/lena.bmp
	@bash -c 'echo -e "\x1b[32;1mSuccess, binary generated: $(OUTPUT_DIR)/$(BIN)\x1b[0m"'

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

# Generate a REL file for each source file. In fact, SDCC doesn't support compiling multiple source file
# at once. We have to create the same directory structure in output dir too.
$(SRCS_REL): $(OUTPUT_DIR)/%.rel : $(INPUT_DIR)/%.c
	@mkdir -p $(OUTPUT_DIR)/$(dir $*)
	$(CC) $(CFLAGS) -o $(OUTPUT_DIR)/$(dir $*) $<

# Generate the final Intel HEX binary.
$(OUTPUT_DIR)/$(BIN_HEX): $(CRT_REL) $(SRCS_REL)
	$(LD) $(LDFLAGS) $(OUTPUT_DIR)/$(BIN_HEX) $(CRT_REL) $(SRCS_REL)

# Convert the Intel HEX file to an actual binary.
$(OUTPUT_DIR)/$(BIN):
	$(OBJCOPY) --input-target=ihex --output-target=binary $(OUTPUT_DIR)/$(BIN_HEX) $(OUTPUT_DIR)/$(BIN)

debug_img:
	rm $(ZFS_PATH)/zim_dbg.img
	$(ZFS_PATH)/zealfs --image=$(ZFS_PATH)/zim_dbg.img --size=64 $(ZFS_PATH)/zfs_mnt
	cp $(LENA) $(ZFS_PATH)/zfs_mnt/lena.bmp
	cp $(OUTPUT_DIR)/$(BIN) $(ZFS_PATH)/zfs_mnt/
	umount $(ZFS_PATH)/zfs_mnt

clean:
	rm -fr bin/
