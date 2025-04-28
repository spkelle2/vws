import gurobi as gu
import os
import pandas as pd

problem_set = "miplib_2017_5000"
problem_folder = os.path.join("test_sets", problem_set)

dimensions = {}

# crawl over each base instance
for base_instance in os.listdir(problem_folder):
    instance_folder = os.path.join(problem_folder, base_instance)
    if not os.path.isdir(instance_folder):
        continue

    # get a folder in os.listdir(instance_folder) corresponding to an experiment
    experiment_folder = [
        folder for folder in os.listdir(instance_folder)
        if os.path.isdir(os.path.join(instance_folder, folder)) and
        any(perturbation in folder for perturbation in ["matrix", "objective", "rhs", "bound"])
    ]

    # skip the base instance if its folder is empty
    if not experiment_folder:
        continue

    # get an mps file from the experiment folder
    mps_file = [file for file in os.listdir(os.path.join(instance_folder, experiment_folder[0]))
                if file.endswith(".mps")][0]
    m = gu.read(os.path.join(instance_folder, experiment_folder[0], mps_file))
    dimensions[base_instance] = {
        "rows": m.NumConstrs,
        "columns": m.NumVars
    }

# convert dimensions to a pandas DataFrame
df = pd.DataFrame.from_dict(dimensions, orient="index").reset_index()
df.columns = ["instance", "rows", "columns"]

# save the DataFrame to a csv file
df.to_csv(f"{problem_set}_sizes.csv", index=False)

