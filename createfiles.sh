#!/bin/bash

gid="166794"
firstname="Sarthak"
lastname="Bosamiya"

read -p "Enter the Module name: " module
dir_path="./${module}/src"

read -p "Enter the number of C files to create: " num_files

read -p "Enter the Task Number: " task_num

base_name="${gid}_${firstname}${lastname}_${module}_T${task_num}"

current_date=$(date +"%d-%m-%Y")

content=$(cat <<EOF
// Question

/** REQUIRED HEADER FILES **/

#include<stdio.h>
#include"../include/head.h"

/*
 * Name of the function:
 * Author: ${firstname} ${lastname}
 * Created: $current_date
 * Modified: $current_date
*/
EOF
)

# Create src directory
if [ ! -d "$dir_path" ]; then
    echo "Directory does not exist. Creating directory: $dir_path ...................."
    mkdir -p "$dir_path"
fi

# Create C source files
for ((i=1; i<=num_files; i++))
do
    file_name="${dir_path}/${base_name}_${i}.c"
    echo -e "$content" > "$file_name"
    echo "Created: $file_name .................."
done

# Create main.c
echo -e "$content" > "${dir_path}/main.c"
echo "Created: ${dir_path}/main.c .................."

# Create include directory and head.h
head_dir="./${module}/include"
mkdir -p "$head_dir"
head_file="${head_dir}/head.h"
head_content=$(cat <<EOF
#ifndef HEAD_H
#define HEAD_H


#endif
EOF
)
echo -e "$head_content" > "$head_file"
echo "Created: $head_file .................."

# Create bin and executable directories
mkdir -p "./${module}/bin"
mkdir -p "./${module}/executable"
echo "Directories for object files (bin) and executables are created.............."

# Copy Makefile into the module directory
cat <<'EOF' > "./${module}/Makefile"
# target: prerequisites

DEBUG=1
EXECUTABLE_NAME=main

CC=gcc
CFLAGS= -Wall
LDFLAGS=

SRC_DIR = src
BIN_DIR = bin

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst %.c, $(BIN_DIR)/%.o, $(notdir $(SRC_FILES)))

ifeq ($(DEBUG),1)
	CFLAGS += -g -O0
else
	CFLAGS = -O3
endif

COMPILER_CALL= $(CC) $(CFLAGS)

target:	build	clean

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(COMPILER_CALL) -c $< -o $@

build: $(OBJ_FILES)
	$(COMPILER_CALL) $(OBJ_FILES) $(LDFLAGS) -o $(EXECUTABLE_NAME)

clean:
	mv $(EXECUTABLE_NAME) ./executable/$(EXECUTABLE_NAME)
EOF

echo "Makefile is created and copied to ./${module}/Makefile .........."

