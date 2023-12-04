import shutil
import os

# Directory containing the files to be copied
destination_directory = 'miplib_5000'

# Read the list of filenames from mips.txt
with open('mips.txt', 'r') as file:
    filenames = file.read().splitlines()

# Copy the files to the current directory
for filename in filenames:
    source_path = os.path.join('/Users/sean/Documents/school/phd/research/warm_starting/miplib3', f"{filename}.mps")
    if os.path.isfile(source_path):
        shutil.copy2(source_path, os.path.join(destination_directory, f"{filename}.mps"))
