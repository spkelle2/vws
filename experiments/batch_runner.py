import os
import subprocess
import sys


def run_batch(test_fldr: str, remote: bool = True, max_time: int = 300,
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

    # make sure we have a valid mip solver
    assert mip_solver in ["CBC", "GUROBI"], "mip_solver must be either CBC or GUROBI"

    # get the output folder and make sure it does not yet exist, then make it
    output_fldr = os.path.join(os.getcwd(), "results", test_fldr)
    os.makedirs(output_fldr, exist_ok=True)

    # iterate over instances, their perturbations, disjunctive terms, and generators
    for instance in os.listdir(input_fldr):
        if not os.path.isdir(os.path.join(input_fldr, instance)):
            continue
        for perturbation in os.listdir(os.path.join(input_fldr, instance)):
            if not os.path.isdir(os.path.join(input_fldr, instance, perturbation)):
                continue
            for terms in [4, 16, 64]:
                for generator in ["None", "New", "Old", "Farkas"]:

                    # get the path to folder with the series to run and where to save the output
                    test_name = f"{instance}_{perturbation}_{terms}_{generator}"
                    stem = os.path.join(output_fldr, test_name)
                    series_input_fldr = os.path.join(input_fldr, instance, perturbation)

                    # skip if the output already exists
                    if os.path.exists(stem + ".csv"):
                        continue

                    if remote:
                        # submit the job to the cluster
                        args = f'INPUT_FOLDER={series_input_fldr},OUTPUT_FILE={stem+".csv"},'\
                            f'MAX_TIME={max_time},GENERATOR={generator},TERMS={terms},' \
                            f'MIP_SOLVER={mip_solver},PROVIDE_PRIMAL_BOUND={int(provide_primal_bound)}'
                        subprocess.call(
                            ['qsub', '-V', '-q', 'batch', '-l', 'ncpus=2,mem=4gb,vmem=4gb,pmem=4gb',
                             '-v', args, '-e', f'{stem}.err', '-o', f'{stem}.out',
                             '-N', test_name, 'submit.pbs']
                        )
                    else:
                        # run locally
                        subprocess.call(["../Debug/vws", series_input_fldr, stem + ".csv",
                                         str(max_time), generator, str(terms),
                                         mip_solver, str(int(provide_primal_bound))])

if __name__ == '__main__':
    run_batch(sys.argv[1], mip_solver="GUROBI")
