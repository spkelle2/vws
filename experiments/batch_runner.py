from math import ceil
import numpy as np
import os
import pandas as pd
import subprocess
import sys


def get_queue(time_limit):
    """ Get the queue to submit to based on the time limit

    :param time_limit: the time limit required in hours
    :return: the queue to submit to
    """
    if time_limit <= 1:
        return "batch"
    elif time_limit <= 2:
        return "short"
    elif time_limit <= 4:
        return "medium"
    else:
        return "long"


def run_batch(test_fldr: str, machine: str = "coral", max_time: int = 3600,
              mip_solver: str = "CBC", provide_primal_bound: bool = True,
              queue_limit: int = 8000):
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

    # track number of jobs
    total_jobs, complete_jobs, submit_jobs = 0, 0, 0

    # get the output folder and make sure it does not yet exist, then make it
    output_fldr = os.path.join(os.getcwd(), "results", test_fldr)
    os.makedirs(output_fldr, exist_ok=True)

    # read the memory required for each instance
    mem_df = pd.read_csv("more_memory.csv", index_col=0)

    # read the strings in cbc.txt into a list
    with open("cbc.txt", "r") as f:
        cbc_instances = f.readlines()

    # iterate over instances, their perturbations, disjunctive terms, and generators
    for instance in os.listdir(input_fldr):
        if not os.path.isdir(os.path.join(input_fldr, instance)):
            continue
        if mip_solver == "CBC" and instance+'\n' not in cbc_instances:
            continue
        # partial: "neos-933562", "timtab2"

        # get the memory required for this instance
        instance_file = instance + ".mps"
        mem = int(mem_df.loc[instance_file, 'memory']) if instance_file in mem_df.index else 4
        print(f"instance {instance} requires {mem}gb")

        for perturbation in os.listdir(os.path.join(input_fldr, instance)):
            if not os.path.isdir(os.path.join(input_fldr, instance, perturbation)):
                continue

            for terms in [4, 16, 64]:
                for generator in ["None", "New", "Old", "Farkas"]:

                    # increment the total number of jobs
                    total_jobs += 1

                    # get the path to folder with the series to run and where to save the output
                    test_name = f"{instance}_{perturbation}_{terms}_{generator}"
                    stem = os.path.join(output_fldr, test_name)
                    series_input_fldr = os.path.join(input_fldr, instance, perturbation)
                    num_mips = len([f for f in os.listdir(series_input_fldr) if f.endswith(".mps")])
                    total_time_limit = ceil(2 * max_time * num_mips / 3600) + 1 # hours
                    queue = get_queue(total_time_limit)

                    # skip if the output already exists
                    if os.path.exists(stem + ".csv") or os.path.exists(stem + ".err"):
                        print(f"{test_name} already ran")
                        complete_jobs += 1
                        continue

                    # skip if we've hit the queue limit
                    if submit_jobs >= queue_limit:
                        continue

                    remote_args = f'INPUT_FOLDER={series_input_fldr},OUTPUT_FILE={stem + ".csv"},' \
                        f'MAX_TIME={max_time},GENERATOR={generator},TERMS={terms},' \
                        f'MIP_SOLVER={mip_solver},PROVIDE_PRIMAL_BOUND={int(provide_primal_bound)}'
                    if machine == "coral":
                        # submit the job to the cluster
                        resources = f'ncpus=1,mem={mem}gb,vmem={mem}gb,pmem={mem}gb,walltime={total_time_limit}:00:00'
                        subprocess.call(
                            ['qsub', '-V', '-q', "urgent", '-l', resources,
                             '-v', remote_args, '-e', f'{stem}.err', '-o', f'{stem}.out',
                             '-N', test_name, 'submit.pbs']
                        )
                    elif machine == "sol":
                        subprocess.call([
                            "sbatch", f"--job-name={test_name}", f"--output={stem}.out",
                            f"--error={stem}.err", f"--time={total_time_limit * 60}", "--ntasks=1",
                            "--cpus-per-task=1", f"--mem={mem}G", "--partition=engi",
                            f"--export={remote_args}", "submit.sh"
                        ])
                    else:
                        # run locally
                        local_args = ["../Debug/vws", series_input_fldr, stem + ".csv",
                                      str(max_time), generator, str(terms),
                                      mip_solver, str(int(provide_primal_bound))]
                        print(" ".join(local_args))
                        subprocess.call(local_args)

                    submit_jobs += 1

                    # exit if we've hit the queue limit
                    if submit_jobs >= queue_limit:
                        print("Queue limit reached")

    print(f"{total_jobs} total jobs")
    print(f"{complete_jobs} already ran")
    print(f"{submit_jobs} submitted")
    print(f"{total_jobs - complete_jobs - submit_jobs} remaining")


if __name__ == '__main__':
    run_batch(sys.argv[1], mip_solver="GUROBI", machine="coral")
