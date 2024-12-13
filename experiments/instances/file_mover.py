import gurobi as gu
import os
import shutil


# iterate over all folders
for fldr in ["miplib2017"]:  # "coral", "miplib2", "miplib3", "miplib2003", "miplib2010",

    # get source and destination directories
    source_directory = f'/Users/sean/Documents/school/phd/research/warm_starting/{fldr}'
    destination_directory = 'miplib_2017_5000_v2'

    # Copy the files to the current directory
    for filename in os.listdir(source_directory):

        # get the source and destination paths
        source_path = os.path.join(source_directory, filename)
        dest_path = os.path.join(destination_directory, filename)

        # if we have a file that isn't a MIP or has already been copied, skip it
        if not filename.endswith(".mps") or os.path.isfile(dest_path):
            continue

        # check if the file is a presolved MIP with less than 5000 rows and columns
        try:
            mdl = gu.read(source_path).presolve()
        except gu.GurobiError:
            print(f"gurobi could not read and/or presolve {filename}")
            continue
        if mdl.NumVars > 5000 or mdl.NumConstrs > 5000:
            continue

        # we have a candidate file, so copy it!
        print(f"copying {filename}!")
        shutil.copy2(source_path, dest_path)
