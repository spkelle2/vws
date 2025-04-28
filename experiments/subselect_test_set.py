"""
selects a random subset of instances. used as input to copy_test_sets.py
"""

import random

source_file = "miplib_2017_names.txt"
dest_file = "miplib_2017_subset_names.txt"


# Open the input file for reading
with open(source_file, "r") as input_file:
    # Read all lines from the input file
    lines = input_file.readlines()

# Open the output file for writing
with open(dest_file, "w") as output_file:
    # Iterate over each line in the input file
    for line in lines:
        # Generate a random number between 0 and 1
        random_number = random.random()
        # If the random number is less than or equal to 0.1 (probability of 0.1)
        if random_number <= 0.1:
            # Write the line to the output file
            output_file.write(line)
