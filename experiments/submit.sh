#!/bin/bash
#SBATCH --job-name=$TEST_NAME
#SBATCH --output=STEM.out
#SBATCH --error=$STEM.err
#SBATCH --time=1:00:00
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=4G

cd /home/sek519/warm_starting/vws/experiments
source /home/sek519/.bashrc
source /home/sek519/miniconda/bin/activate
conda activate vws

# Your command
../Debug/vws ${INPUT_FOLDER} ${OUTPUT_FILE} ${MAX_TIME} ${GENERATOR} ${TERMS} ${MIP_SOLVER} ${PROVIDE_PRIMAL_BOUND}