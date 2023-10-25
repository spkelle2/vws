import os
import subprocess
import sys


def run_batch(test_fldr: str, remote: bool = True, max_time: int = 300):
    """ For all problems and perturbations, run the .mps associated with each series

    :param test_fldr: directory containing directories of instances which in turn
    contains directories of series corresponding to the perturbations
    :return: None
    """

    # get the input folder and make sure it exists
    input_fldr = os.path.join(os.getcwd(), "test_sets", test_fldr)
    assert os.path.exists(input_fldr), f'{test_fldr} must belong in the "test_sets" ' \
        f'folder and the path {input_fldr} must exist'

    # get the output folder and make sure it does not yet exist, then make it
    output_fldr = os.path.join(os.getcwd(), "results", test_fldr)
    os.makedirs(output_fldr, exist_ok=False)

    # iterate over instances, their perturbations, disjunctive terms, and generators
    for instance in os.listdir(input_fldr):
        if not os.path.isdir(os.path.join(input_fldr, instance)):
            continue
        for perturbation in os.listdir(os.path.join(input_fldr, instance)):
            if not os.path.isdir(os.path.join(input_fldr, instance, perturbation)):
                continue
            for terms in [4, 8, 16, 32, 64, 128, 256]:
                for generator in ["None", "New Disjunction", "Old Disjunction", "Farkas"]:

                    # get the path to folder with the series to run and where to save the output
                    stem = os.path.join(output_fldr, f"{instance}_{perturbation}_{terms}_{generator}")
                    series_input_fldr = os.path.join(input_fldr, instance, perturbation)

                    if remote:
                        # submit the job to the cluster
                        args = f'INPUT_FOLDER={series_input_fldr},OUTPUT_FILE={stem+".csv"},'\
                            f'MAX_TIME={max_time},GENERATOR={generator},TERMS={terms}'
                        subprocess.call(
                            ['qsub', '-V', '-q', 'medium', '-l', 'ncpus=8,mem=16gb,vmem=16gb,pmem=16gb',
                             '-v', args, '-e', f'{stem}.err', '-o', f'{stem}.out', 'submit.pbs']
                        )
                    else:
                        # run locally
                        subprocess.call(["../Debug/vws", series_input_fldr, stem + ".csv",
                                         str(max_time), generator, str(terms)])

if __name__ == '__main__':
    run_batch(sys.argv[1], True, 300)
