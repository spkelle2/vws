import gurobipy as gp
import numpy as np
import os
from scipy.sparse import csr_matrix
import shutil
import sys
import time
from typing import Union


def angle_diff(v1: np.ndarray, v2: np.ndarray) -> float:
    theta = min(1, max(-1, np.dot(v1, v2) / (np.linalg.norm(v1) * np.linalg.norm(v2))))
    return np.degrees(np.arccos(theta))


def norm_diff(v1: np.ndarray, v2: np.ndarray) -> float:
    return abs(np.linalg.norm(v1) - np.linalg.norm(v2))


def perturb(vector: np.ndarray, p: int, unit: int = 1, prioritize=False) -> Union[np.ndarray, None]:
    """This function perturbs a vector by 2^p degrees and within 2^p % of its norm.
    It prioritizes perturbing non-integer, non-binary, and then binary values.

    :param vector: the vector to be perturbed
    :param p: the power of 2 to perturb the vector by
    :param unit: the increment of element perturbation
    :return: the perturbed vector
    """
    assert isinstance(vector, np.ndarray), "vector should be a numpy array"
    assert len(vector.shape) == 1, "vector should be 1-D"

    def is_binary(val):
        return np.isclose(val, 0) or np.isclose(val, 1)

    def is_integer(val):
        return np.isclose(val % 1, 0)

    new_vector = vector.copy()
    prev_vector = None

    # Categorize indices
    non_integer_indices = [i for i, val in enumerate(vector) if not is_integer(val)]
    non_binary_indices = [i for i, val in enumerate(vector) if is_integer(val) and not is_binary(val)]
    binary_indices = [i for i, val in enumerate(vector) if is_binary(val)]

    while angle_diff(new_vector, vector) < 2 ** p and norm_diff(new_vector, vector) < 2 ** p:
        prev_vector = new_vector.copy()

        if prioritize:
            # Choose perturbation index based on priority
            if non_integer_indices:
                index = np.random.choice(non_integer_indices)
            elif non_binary_indices:
                index = np.random.choice(non_binary_indices)
            else:
                index = np.random.choice(binary_indices)
        else:
            # otherwise just take a random index
            index = np.random.randint(new_vector.shape[0])

        # Apply random perturbation
        new_vector[index] += np.random.choice([-unit, unit])

    if all(prev_vector == vector):
        return None
    else:
        return prev_vector


def perturb_bounds(mdl: gp.Model, p: int) -> Union[tuple[list, list], tuple[None, None]]:
    """This function perturbs the bounds of the model's binary variables by
    2^p degrees and within 2^p % of its norm. It does so by tightening (or
    loosening where applicable) the bounds at random by 1 until the perturbation
    is achieved.

    :param mdl: the model to be perturbed
    :param p: the power of 2 to perturb the model by
    :return: the perturbed model
    """
    # maybe randomly add or subtract 1 until we get to 2^p degrees or 2^p % of the norm difference
    assert isinstance(mdl, gp.Model), "mdl should be a gurobi model"

    # get initial bounds and copy to track last change
    orig_l, orig_u = np.array(mdl.getAttr('LB')), np.array(mdl.getAttr('UB'))
    l, u = orig_l.copy(), orig_u.copy()
    prev_l, prev_u = None, None

    # get helpful indices
    bin_vars = [i for i, v in enumerate(mdl.getVars()) if v.vtype in
                [gp.GRB.BINARY, gp.GRB.INTEGER] and l[i] == 0 and u[i] == 1]

    # tighten bounds until we get to the perturbation limit
    while angle_diff(np.concatenate([l[bin_vars], u[bin_vars]]),
                     np.concatenate([orig_l[bin_vars], orig_u[bin_vars]])) < 2 ** p and \
            norm_diff(np.concatenate([l[bin_vars], u[bin_vars]]),
                      np.concatenate([orig_l[bin_vars], orig_u[bin_vars]])) < 2 ** p:

        # make a copy of the current bounds in case we go past the perturbation limit this time
        prev_l, prev_u = l.copy(), u.copy()

        # decide which way to tighten
        if np.random.uniform() < .5:
            # increment a lower bound from those that are still adjustable
            choices = [i for i in bin_vars if l[i] < u[i]]
            idx = np.random.choice(choices)
            l[idx] += 1

        else:
            # change an upper bound from those that are still adjustable
            choices = [i for i in bin_vars if l[i] < u[i]]
            idx = np.random.choice(choices)
            u[idx] -= 1

    if all(prev_l == orig_l) and all(prev_u == orig_u):
        return None, None
    else:
        return prev_l, prev_u


def write_objective(stem: str, val: float):
    with open(f"{stem}.pb", 'w') as file:
        file.write(str(val))


def keep_perturbing(count: dict, samples: int):
    """ keep perturbing if we do not have enough samples """
    return sum(v - 1 for v in count.values()) < samples


def presolve_instance(base_mdl: gp.Model, instance_name: str, max_vars: int, max_cons: int,
                      perturbation: str = None, p: int = None) -> Union[gp.Model, None]:
    """ presolve the instance and return the presolved instance if it is within the size limits """

    try:
        presolved_base_mdl = base_mdl.presolve()
    except gp.GurobiError:
        # model could not be presolved
        presolved_base_mdl = base_mdl

    if presolved_base_mdl.numConstrs > max_cons or presolved_base_mdl.numVars > max_vars:
        warn_str = f"{instance_name} presolve failed to reduce to maximum size"
        if perturbation and p:
            warn_str = "Warning: " + warn_str + f" for {perturbation} perturbation and degree {p}"
            print(warn_str)
        else:
            print(warn_str, file=sys.stderr)
        return
    else:
        return presolved_base_mdl


def try_solving(presolved_tmp_mdl, perturbation, p, perturbed_instance_dir, instance_name, count, extension):
    """ try solving the perturbed instance and write it if it is solvable """

    presolved_tmp_mdl.setParam("TimeLimit", 3600)
    presolved_tmp_mdl.setParam('OutputFlag', 0)
    presolved_tmp_mdl.optimize()
    if presolved_tmp_mdl.status == gp.GRB.OPTIMAL:
        stem = os.path.join(perturbed_instance_dir, f"{perturbation}_{p}",
                            f"{instance_name}_{count[perturbation]}")
        presolved_tmp_mdl.write(f"{stem}{extension}")
        write_objective(stem, presolved_tmp_mdl.objVal)
        count[perturbation] += 1
    else:
        print(f"Warning: {instance_name} could not be solved for {perturbation} perturbation and degree {p}")


def make_instance_set(instance_file, instances_fldr: str, p: int, samples: int = 3,
                      max_vars: int = 5000, max_cons: int = 5000):
    """ make the test set for a single instance

    :param instance_file: the saved instance
    :param instances_fldr: folder where saved instance can be found
    :param p: the power of 2 to perturb the instance by
    :param samples: number of samples to make of each perturbation
    :param max_vars: maximum number of variables in the presolved instance
    :param max_cons: maximum number of constraints in the presolved instance
    :return: None
    """

    assert isinstance(samples, int) and samples > 0, "samples should be a positive integer"
    assert isinstance(p, int), "p should be an integer"

    # get information about the instance location
    instance_name, extension = os.path.splitext(instance_file)
    # where is the original instance
    instance_pth = os.path.join('instances', instances_fldr, instance_file)
    # in what directory should the perturbed instances be stored
    perturbed_instance_dir = os.path.join('test_sets', instances_fldr, instance_name)
    # filter out non .mps files
    if extension != '.mps':
        return
    # make a directory to hold the series for this instance if it doesn't exist
    if not os.path.exists(perturbed_instance_dir):
        os.makedirs(perturbed_instance_dir, exist_ok=True)

    # read in the instance
    base_mdl = gp.read(instance_pth)
    base_mdl.setParam("TimeLimit", 3600)
    base_mdl.setParam('OutputFlag', 0)

    # presolve it, and skip if it is too large
    presolved_base_mdl = presolve_instance(base_mdl, instance_name, max_vars, max_cons)
    if not presolved_base_mdl:
        return

    # get the optimal presolved primal bound, and skip if it is solved to optimality
    presolved_base_mdl.optimize()
    if presolved_base_mdl.status != gp.GRB.OPTIMAL:
        print(f"{instance_name} could not be solved in 3600 seconds. skipping.",
              file=sys.stderr)
        return
    objective_value = presolved_base_mdl.objVal

    start_time = time.time()
    max_duration = 23 * 60 * 60  # 23 hours in seconds
    max_tries = 100
    count = {"objective": 1, "rhs": 1, "matrix": 1}
    for kind in count:
        # create a directory for each kind of perturbation of this instance
        series_fldr = os.path.join(perturbed_instance_dir, f"{kind}_{p}")
        if not os.path.exists(series_fldr):
            os.mkdir(series_fldr)
        else:
            # get a count of the number of existing test instances
            count[kind] = len([f for f in os.listdir(series_fldr) if f.endswith(extension) and not f.startswith("._")])
            if count[kind] > 0:
                continue
        # Copy and rename the presolved base instance as the first instance - should help with reducing tree size in VPC
        stem = os.path.join(series_fldr, f"{instance_name}_0")
        presolved_base_mdl.write(f"{stem}{extension}")
        # save its objective value
        write_objective(stem, objective_value)

    # make a bunch of random perturbations of the instance until hopefully we get <sample> feasible ones
    iterations = 0
    while any(v - 1 < samples for v in count.values()) and \
            time.time() - start_time < max_duration and iterations < max_tries:
        # update termination condition
        iterations += 1

        # perturb the constraint matrix
        A, unit = None, 1
        while A is None and unit > 1e-6:
            coefs = perturb(np.array(presolved_base_mdl.getA().data), p, unit)
            A = csr_matrix((coefs, presolved_base_mdl.getA().nonzero()), presolved_base_mdl.getA().shape) \
                if coefs is not None else None
            unit *= .5
        if A is None:
            print(f"Warning: {instance_name} constraint matrix could not be perturbed for p = {p}")

        # perturb the rhs
        b, unit = None, 1
        while b is None and unit > 1e-6:
            b = perturb(np.array(presolved_base_mdl.getAttr('RHS')), p, unit)
            unit *= .5
        if b is None:
            print(f"Warning: {instance_name} rhs could not be perturbed for p = {p}")

        # perturb the objective
        c, unit = None, 1
        while c is None and unit > 1e-6:
            c = perturb(np.array(presolved_base_mdl.getAttr('OBJ')), p, unit)
            unit *= .5
        if c is None:
            print(f"Warning: {instance_name} objective could not be perturbed for p = {p}")

        # write the objective perturbation if it presolves to our expected size and is solvable
        if c is not None and count["objective"] - 1 < samples:
            tmp_mdl = presolved_base_mdl.copy()
            for j, coef in enumerate(c):
                tmp_mdl.getVars()[j].Obj = coef
            try_solving(tmp_mdl, "objective", p, perturbed_instance_dir, instance_name, count, extension)

        # write the rhs perturbation if it is solvable
        if b is not None and count["rhs"] - 1 < samples:
            tmp_mdl = presolved_base_mdl.copy()
            for i, coef in enumerate(b):
                tmp_mdl.getConstrs()[i].Rhs = coef
            try_solving(tmp_mdl, "rhs", p, perturbed_instance_dir, instance_name, count, extension)

        # write the matrix perturbation if it is solvable
        if A is not None and count["matrix"] - 1 < samples:
            tmp_mdl = presolved_base_mdl.copy()
            for i, j in zip(*A.nonzero()):
                tmp_mdl.chgCoeff(tmp_mdl.getConstrs()[i], tmp_mdl.getVars()[j], A[i, j])
            try_solving(tmp_mdl, "matrix", p, perturbed_instance_dir, instance_name, count, extension)

        if iterations % 50 == 0:
            print(f"after {iterations} iterations, {instance_name} has {count} samples for p = {p}")

    for kind, amount in count.items():
        if amount - 1 < samples:
            print(f"{instance_name} has {amount - 1} samples of {kind} for p = {p}", file=sys.stderr)
        if amount == 1:
            # delete the folder if there are no perturbations
            shutil.rmtree(os.path.join(perturbed_instance_dir, f"{kind}_{p}"))


if __name__ == '__main__':
    make_instance_set(sys.argv[1], sys.argv[2], int(sys.argv[3]), samples=int(sys.argv[4]))
