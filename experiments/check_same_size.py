import os
import sys

import gurobipy as gp
from gurobipy import GRB

def find_mps_files(directory):
    """Recursively find all .mps files in a given directory."""
    mps_files = []
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(".mps"):
                mps_files.append(os.path.join(root, file))
    return mps_files

def load_mps_file(file_path):
    """Load an .mps file into a Gurobi model."""
    try:
        model = gp.read(file_path)
        return model
    except gp.GurobiError as e:
        print(f"Error reading {file_path}: {e}")
        return None

def check_models_consistency(mps_files):
    """Check if all models have the same number of variables and constraints."""
    if not mps_files:
        print("No .mps files found.")
        return

    base_vars = None
    base_constraints = None

    for file_path in mps_files:
        print(f"Processing {file_path}...")
        model = load_mps_file(file_path)

        if model is None:
            continue

        num_vars = model.NumVars
        num_constraints = model.NumConstrs

        if base_vars is None and base_constraints is None:
            base_vars = num_vars
            base_constraints = num_constraints
            print(f"Base model variables: {base_vars}, constraints: {base_constraints}")
        else:
            if num_vars != base_vars or num_constraints != base_constraints:
                return False

    return True

def main(directory_name):
    directory = os.path.join("test_sets", directory_name)

    if not os.path.isdir(directory):
        print("Invalid directory.")
        return

    inconsistent = []
    for instance_name in os.listdir(directory):

        instance_directory = os.path.join(directory, instance_name)
        if not os.path.isdir(instance_directory):
            continue

        print(f"Checking {instance_directory}...")

        mps_files = find_mps_files(instance_directory)
        if not check_models_consistency(mps_files):
            inconsistent.append(instance_name)

    print(f"Inconsistent instances: {inconsistent}")

if __name__ == "__main__":
    main(sys.argv[1])