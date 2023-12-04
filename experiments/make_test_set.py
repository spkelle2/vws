import os
import subprocess

from make_test_instance import make_instance_set


def main(instances_fldr, remote: bool = True):
    """This function creates a test set of instances for the experiments.

    :param instances_fldr: the name of the folder in the instances directory
    containing the instances to be perturbed
    :param samples: how many perturbations of each instance to make
    :param perturbations: a list powers of 2 (in degrees and % change of norm)
    to perturb the instances by
    :return: None
    """

    # input checks
    assert os.path.exists(os.path.join('instances', instances_fldr)),\
        f'"instances/{instances_fldr}" should exist'
    assert not os.path.exists(os.path.join('test_sets', instances_fldr)),\
        f'"test_sets/{instances_fldr}" directory should not exist'
    os.mkdir(os.path.join('test_sets', instances_fldr))

    # iterate over all files in the instances directory
    for instance_idx, instance_file in enumerate(os.listdir(os.path.join('instances', instances_fldr))):
        instance_name, extension = os.path.splitext(instance_file)
        print(f"instance {instance_idx + 1} of {len(os.listdir(os.path.join('instances', instances_fldr)))}")

        if remote:
            # submit the job to the cluster
            args = f'INSTANCE_FILE={instance_file},INSTANCES_FLDR={instances_fldr}'
            subprocess.call(
                ['qsub', '-V', '-q', 'long', '-l', 'ncpus=2,mem=4gb,vmem=4gb,pmem=4gb',
                 '-v', args, '-e', f'{instance_name}.err', '-o', f'{instance_name}.out',
                 '-N', instance_name, 'submit_creation.pbs']
            )
        else:
            # run locally
            subprocess.call(["python", "make_test_instance.py", instance_file, instances_fldr])


if __name__ == '__main__':
    main(instances_fldr="bm23", remote=False)
