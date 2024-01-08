import os
import shutil

test_set_path = "test_sets/miplib_5000"
instances_path = "instances/miplib_5000"

# Get a list of folders in test_set/miplib_5000
folders_to_check = [folder for folder in os.listdir(test_set_path) if
                    os.path.isdir(os.path.join(test_set_path, folder))]

for folder in folders_to_check:
    # Construct the path to the corresponding .mps file in instances/miplib_5000
    mps_file_path = os.path.join(instances_path, folder + ".mps")

    # Check if the .mps file exists
    if os.path.exists(mps_file_path):
        print(f".mps file found for folder {folder}. Keeping.")
    else:
        # If .mps file doesn't exist, delete the folder
        folder_path = os.path.join(test_set_path, folder)
        shutil.rmtree(folder_path)
        print(f"Deleted folder {folder} as no corresponding .mps file was found.")
