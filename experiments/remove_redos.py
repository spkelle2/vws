import pandas as pd
import os

# Assuming more_memory.csv has a column named "filename"
csv_file = "more_memory.csv"

# Read the CSV file into a pandas DataFrame
df = pd.read_csv(csv_file, index_col=0)

# Iterate over each file name
for filename in df.index:

    # Construct the corresponding ".err" file name
    err_file = os.path.splitext(filename)[0] + ".err"
    out_file = os.path.splitext(filename)[0] + ".out"

    # Check if the ".err" file exists before attempting to delete
    if os.path.exists(err_file):
        # Delete the ".err" file
        os.remove(err_file)
        print(f"Deleted {err_file}")
    else:
        print(f"{err_file} not found")

    if os.path.exists(out_file):
        # Delete the ".out" file
        os.remove(out_file)
        print(f"Deleted {out_file}")
    else:
        print(f"{out_file} not found")