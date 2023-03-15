import os
import subprocess
import sys


def run_batch(test_fldr: str):
    """ Run all the .test files in the test_fldr folder

    :param test_fldr: location of the .test files
    :return: None
    """

    assert os.path.exists(test_fldr), f'the path {test_fldr} does not exist'

    for file_name in os.listdir(test_fldr):
        stem = file_name.split('.')[0]
        file_pth = os.path.join(test_fldr, file_name)

        if not file_name.endswith('.test'):
            continue

        subprocess.call(
            ['qsub', '-V', '-q', 'long', '-l', 'ncpus=1,mem=15gb,vmem=15gb,pmem=15gb',
             '-v', f'file_pth={file_pth}', '-e', f'{stem}.err', '-o', f'{stem}.out',
             'submit.pbs']
        )


if __name__ == '__main__':
    run_batch(sys.argv[1])
