# Only fetched xD

import sys

if len(sys.argv) != 3:
    print("Usage: python script.py <input_file> <output_file>")
    sys.exit(1)

input_file = sys.argv[1]
output_file = sys.argv[2]

with open(input_file, "r") as infile, open(output_file, "w") as outfile:
    for line in infile:
        line = line.strip()
        if "Fetched ins" in line:
            # Remove the <number> prefix
            cleaned_line = line.split(">", 1)[-1].strip()
            outfile.write(cleaned_line + "\n")

