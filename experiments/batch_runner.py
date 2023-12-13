import os
import subprocess
import sys


def run_batch(test_fldr: str, machine: str = "coral", max_time: int = 300,
              mip_solver: str = "CBC", provide_primal_bound: bool = True):
    """ For all problems and perturbations, run the .mps associated with each series

    :param test_fldr: directory containing directories of instances which in turn
    contains directories of series corresponding to the perturbations
    :return: None
    """

    # get the input folder and make sure it exists
    input_fldr = os.path.join(os.getcwd(), "test_sets", test_fldr)
    assert os.path.exists(input_fldr), f'{test_fldr} must belong in the "test_sets" ' \
        f'folder and the path {input_fldr} must exist'

    # make sure we have a valid machine
    assert machine in ["coral", "local", "sol"], "machine must be coral, local, or sol"

    # make sure we have a valid mip solver
    assert mip_solver in ["CBC", "GUROBI"], "mip_solver must be either CBC or GUROBI"

    # track number of jobs submitted
    count = 0

    # get the output folder and make sure it does not yet exist, then make it
    output_fldr = os.path.join(os.getcwd(), "results", test_fldr)
    os.makedirs(output_fldr, exist_ok=True)

    # read the strings in cbc.txt into a list
    with open("cbc.txt", "r") as f:
        cbc_instances = f.readlines()

    # iterate over instances, their perturbations, disjunctive terms, and generators
    for instance in os.listdir(input_fldr):
        if not os.path.isdir(os.path.join(input_fldr, instance)):
            continue
        if mip_solver == "CBC" and instance+'\n' not in cbc_instances:
            continue

        for perturbation in os.listdir(os.path.join(input_fldr, instance)):
            if not os.path.isdir(os.path.join(input_fldr, instance, perturbation)):
                continue
            # skip large perturbations for now
            attribute, degree = perturbation.rsplit("_")
            degree = int(degree)
            if degree > 1:
                continue

            for terms in [4, 64, 256]:
                for generator in ["None", "New", "Old", "Farkas"]:

                    # get the path to folder with the series to run and where to save the output
                    test_name = f"{instance}_{perturbation}_{terms}_{generator}"
                    stem = os.path.join(output_fldr, test_name)
                    series_input_fldr = os.path.join(input_fldr, instance, perturbation)

                    # skip if the output already exists
                    if os.path.exists(stem + ".csv") or os.path.exists(stem + ".err"):
                        continue

                    coral_args = f'INPUT_FOLDER={series_input_fldr},OUTPUT_FILE={stem + ".csv"},' \
                        f'MAX_TIME={max_time},GENERATOR={generator},TERMS={terms},' \
                        f'MIP_SOLVER={mip_solver},PROVIDE_PRIMAL_BOUND={int(provide_primal_bound)}'
                    if machine == "coral":
                        # submit the job to the cluster
                        subprocess.call(
                            ['qsub', '-V', '-q', 'batch', '-l', 'ncpus=2,mem=4gb,vmem=4gb,pmem=4gb',
                             '-v', coral_args, '-e', f'{stem}.err', '-o', f'{stem}.out',
                             '-N', test_name, 'submit.pbs']
                        )
                    elif machine == "sol":
                        sol_args = coral_args + f",STEM={stem},TEST_NAME={test_name}"
                        subprocess.call(["sbatch", f"--export={sol_args}", "submit.sh"])
                    else:
                        # run locally
                        subprocess.call(["../Debug/vws", series_input_fldr, stem + ".csv",
                                         str(max_time), generator, str(terms),
                                         mip_solver, str(int(provide_primal_bound))])

                    # increment the job counter
                    count += 1

                    # exit if we've hit the queue limit
                    if count >= 4000:
                        return


if __name__ == '__main__':
    run_batch(sys.argv[1], mip_solver="CBC", machine="sol")
