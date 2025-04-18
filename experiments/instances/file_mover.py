import gurobipy as gu
import os
import shutil
import pandas as pd

# Load allowed instance names from CSV
allowed_instances = set(pd.read_csv("quick.csv")["instance"].astype(str))

# Folders to search
folders = ["miplib2017"]  # Add others as needed

# Output directory
destination_directory = "quick"

# Ensure destination exists
os.makedirs(destination_directory, exist_ok=True)

for fldr in folders:
    source_directory = f'/Users/sean/Documents/school/phd/research/warm_starting/{fldr}'

    for filename in os.listdir(source_directory):
        # Skip non-MPS files
        if not filename.endswith(".mps"):
            continue

        instance_name = os.path.splitext(filename)[0]
        if instance_name not in allowed_instances:
            continue

        source_path = os.path.join(source_directory, filename)
        dest_path = os.path.join(destination_directory, filename)

        # Skip if already copied
        if os.path.isfile(dest_path):
            continue

        # Try to presolve and filter large models
        try:
            mdl = gu.read(source_path).presolve()
        except gu.GurobiError:
            print(f"gurobi could not read and/or presolve {filename}")
            continue

        if mdl.NumVars > 5000 or mdl.NumConstrs > 5000:
            continue

        # Copy the valid file
        print(f"copying {filename}!")
        shutil.copy2(source_path, dest_path)