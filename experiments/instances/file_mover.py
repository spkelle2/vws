import shutil
import os

# Directory containing the files to be copied
source_directory = 'miplib_small_easy_binary'

# Read the list of filenames from mips.txt
with open('mips.txt', 'r') as file:
    filenames = file.read().splitlines()

# Copy the files to the current directory
for filename in filenames:
    source_path = os.path.join('/Users/sean/Documents/school/phd/research/warm_starting/miplib', filename)
    if os.path.isfile(source_path):
        shutil.copy2(source_path, os.path.join(source_directory, filename))
