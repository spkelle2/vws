import os
import pandas as pd
import subprocess
import sys

todo = ['neos-4650160-yukon_0',
 'neos-4650160-yukon_2',
 'ran12x21_0',
 'ran12x21_2',
 'neos-4393408-tinui_2',
 'tr12-30_0',
 'tr12-30_4',
 'rout_0',
 'rout_4',
 'ic97_tension_0',
 'ic97_tension_2',
 'neos-3421095-cinca_2',
 'misc07_0',
 'misc07_2',
 'sp98ir_0',
 'sp98ir_4',
 'bienst1_0',
 'bienst1_2',
 'neos-860300_0',
 'neos-860300_2',
 'mtest4ma_0',
 'mtest4ma_2',
 'neos-1445743_2',
 'neos-1445743_4',
 'neos-5051588-culgoa_2',
 'arki001_0',
 'arki001_4',
 'gsvm2rl3_0',
 'gsvm2rl3_2',
 'ran13x13_2',
 'ran13x13_4',
 'mas76_2',
 'mas76_4',
 'aflow30a_2',
 'pg_2',
 'neos-3734794-moppy_2',
 'neos-3734794-moppy_4',
 'neos-585192_2',
 'neos-585192_4']


def main(instances_fldr, remote: bool = True, samples=3):
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

    jobs_submitted = 0

    # make output directory if it doesn't exist
    if not os.path.exists(os.path.join('test_sets', instances_fldr)):
        os.mkdir(os.path.join('test_sets', instances_fldr))

    # read the memory required for each instance
    mem_df = pd.read_csv("more_memory.csv", index_col=0)

    # iterate over all files in the instances directory
    for instance_idx, instance_file in enumerate(os.listdir(os.path.join('instances', instances_fldr))):
        instance_name, extension = os.path.splitext(instance_file)
        print(f"instance {instance_idx + 1} of {len(os.listdir(os.path.join('instances', instances_fldr)))}")

        # get the memory required for this instance
        mem = int(mem_df.loc[instance_file, 'memory']) if instance_file in mem_df.index else 4

        for p in [0, 2, 4]:

            if f"{instance_name}_{p}" not in todo:
                continue

            jobs_submitted += 1
            if remote:
                # submit the job to the cluster
                args = f'INSTANCE_FILE={instance_file},INSTANCES_FLDR={instances_fldr},P={p},SAMPLES={samples}'
                subprocess.call(
                    ['qsub', '-V', '-q', 'mediumlong', '-l', f'ncpus=1,mem={mem}gb,vmem={mem}gb,pmem={mem}gb,walltime=23:59:00',
                     '-v', args, '-e', f'outfiles/{instance_name}_{p}.err', '-o', f'outfiles/{instance_name}_{p}.out',
                     '-N', f'{instance_name}_{p}', 'submit_creation.pbs']
                )
            else:
                # run locally
                subprocess.call(["python", "make_test_instance.py", instance_file,
                                 instances_fldr, str(p), str(samples)])

    print(f"submitted {jobs_submitted} jobs")
    print(f"there are {instance_idx + 1} instances")


if __name__ == '__main__':
    samples = 3 if len(sys.argv) < 3 else int(sys.argv[2])
    main(instances_fldr=sys.argv[1], samples=samples)
