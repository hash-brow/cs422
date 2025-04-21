#!/bin/bash
dirs=("asm-sim" "c-sim" "endian" "factorial" "fib" "hello" "host" "ifactorial" "ifib" "log2" "msort" "rfib" "Subreg" "towers" "vadd")

# Loop through each directory and run the commands
for dir in "${dirs[@]}"; do
    echo "Processing directory: $dir"
    if [ -d "$dir" ]; then
        cd "$dir"
        gmake clobber clean
        gmake
        cd ..
    else
        echo "Directory $dir does not exist!"
    fi
done

