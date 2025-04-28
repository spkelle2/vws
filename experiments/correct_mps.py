import re
import os
import sys


def main(experiment):
    """Remove duplicate variable bounds from the MPS files in the test_sets/<experiment>
    folder arising from perturbations. This is necessary because the bound perturbations
    are created by adding a new constraint to the problem, and the MPS file format
    does not allow for duplicate constraints.
    """

    # get the folder and check it exists
    fldr = f"test_sets/{experiment}"
    assert os.path.exists(fldr), f'{experiment} must belong in the "test_sets" folder'

    # iterate over instances and their bound perturbations
    for instance in os.listdir(fldr):
        instance_pth = os.path.join(fldr, instance)
        if not os.path.isdir(instance_pth):
            continue
        for perturbation in os.listdir(instance_pth):
            perturbation_pth = os.path.join(instance_pth, perturbation)
            # this appears to only affect bound perturbations
            if not os.path.isdir(perturbation_pth) or "bound" not in perturbation:
                continue
            print(f"cleaning bound perturbed problems for {instance}/{perturbation}")

            # check each file in the perturbation folder for duplicate variable bounds
            for file_name in os.listdir(perturbation_pth):
                if not file_name.endswith(".mps"):
                    continue
                file_pth = os.path.join(perturbation_pth, file_name)
                lines_to_delete = []

                with open(file_pth, 'r') as file:
                    lines = file.readlines()

                # match entries like FX BND1      x60935    1
                regex = re.compile(r'(\S+)\s+(\S+)\s+(\S+)\s+(\S+)')

                # check each line if we have a match and if the next line contains the same variable
                for idx, line in enumerate(lines):
                    m = regex.search(line)
                    if m and m.group(1) == "FX" and m.group(3) in lines[idx + 1]:
                        # insert at the beginning to avoid index shifting when deleting
                        lines_to_delete.insert(0, idx + 1)

                # Remove the lines at the specified indexes
                for idx in lines_to_delete:
                    del lines[idx]

                # Write the modified content back to the file
                with open(file_pth, 'w') as file:
                    file.writelines(lines)


if __name__ == "__main__":
    main(sys.argv[1])
