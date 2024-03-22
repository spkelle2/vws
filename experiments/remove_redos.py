import pandas as pd
import os


csv_file = "redos.csv"
results_fldr = "results/miplib_2017_5000"

# Read the CSV file into a pandas DataFrame
df = pd.read_csv(csv_file, index_col=0)

delete_files = 0

# Iterate over each file name
for filename in df.index:

    # Construct the corresponding file names
    err_file = os.path.join(results_fldr, f"{filename}.err")
    out_file = os.path.join(results_fldr, f"{filename}.out")
    csv_file = os.path.join(results_fldr, f"{filename}.csv")

    # Check if the ".err" file exists before attempting to delete
    if os.path.exists(err_file):
        # Delete the ".err" file
        os.remove(err_file)
        delete_files += 1
    else:
        print(f"{err_file} not found")

    if os.path.exists(out_file):
        # Delete the ".out" file
        os.remove(out_file)
        delete_files += 1
    else:
        print(f"{out_file} not found")

    if os.path.exists(csv_file):
        # Delete the ".csv" file
        os.remove(csv_file)
        delete_files += 1
    else:
        print(f"{csv_file} not found")

print(f"Deleted {delete_files} files.")