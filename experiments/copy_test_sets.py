import os
import shutil

instance = False

if not instance:

    # set parameters
    source = "test_sets/miplib_5000"
    dest = "test_sets/miplib_2017_5000"
    file = "miplib_2017_names.txt"

    count = 0

    # read file into a string
    with open(file, "r") as f:
        test_names = f.read().lower()

    for fldr in os.listdir(source):
        if os.path.isdir(os.path.join(source, fldr)) and fldr in test_names:
            print("copying", fldr)
            shutil.copytree(os.path.join(source, fldr), os.path.join(dest, fldr))
            count += 1

    print(count, " folders copied")

else:
    # set parameters
    source = "instances/miplib_5000"
    dest = "instances/miplib_2017_5000"
    fldr = "../../miplib2017"
    fldr_files = os.listdir(fldr)

    count = 0

    for instance in os.listdir(source):
        if instance in fldr_files:
            print("copying", instance)
            shutil.copy(os.path.join(source, instance), os.path.join(dest, instance))
            count += 1

    print(count, " instances copied")