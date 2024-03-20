import os
import shutil

# copy the test sets
# set parameters
source = "test_sets/miplib_2017_5000"
dest = "test_sets/miplib_2017_subset"
source_instances = "instances/miplib_2017_5000"
dest_instances = "instances/miplib_2017_subset"
file = "miplib_2017_subset_names.txt"

count = 0

# read file into a string
with open(file, "r") as f:
    test_names = f.read().lower()

for fldr in os.listdir(source):
    if os.path.isdir(os.path.join(source, fldr)) and fldr in test_names:
        print("copying", fldr)
        shutil.copytree(os.path.join(source, fldr), os.path.join(dest, fldr))
        shutil.copy(os.path.join(source_instances, f"{fldr}.mps"),
                    os.path.join(dest_instances, f"{fldr}.mps"))
        count += 1

print(count, " folders copied")
