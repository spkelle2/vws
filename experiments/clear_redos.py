import pandas as pd
import os
import sys


def main(fldr):
    """
    Delete all files in results/<fldr> contained in redos.csv
    :param fldr:
    :return:
    """
    fldr = os.path.join("results", fldr)

    # check stuff exists
    assert os.path.exists(fldr), f"results/{fldr} does not exist"
    assert os.path.exists("redos.csv"), "redos.csv does not exist"

    # Read the CSV file into list
    csv_file_path = "redos.csv"  # Replace with your actual CSV file path
    df = pd.read_csv(csv_file_path)
    error_list = df["Error Message"].tolist()

    # Delete files based on the error messages
    for redo_stem in error_list:
        files_to_delete = [file for file in os.listdir(fldr) if redo_stem in file]
        for file_to_delete in files_to_delete:
            file_path = os.path.join(fldr, file_to_delete)
            os.remove(file_path)
            print(f"Deleted file: {file_path}")


if __name__ == "__main__":
    main(sys.argv[1])
