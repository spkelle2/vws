import gurobipy as gp
import numpy as np
import os
from scipy.sparse import csr_matrix
import shutil
from typing import Union


def angle_diff(v1: np.ndarray, v2: np.ndarray) -> float:
    theta = min(1, max(-1, np.dot(v1, v2) / (np.linalg.norm(v1) * np.linalg.norm(v2))))
    return np.degrees(np.arccos(theta))


def norm_diff(v1: np.ndarray, v2: np.ndarray) -> float:
    return abs(np.linalg.norm(v1) - np.linalg.norm(v2))


def perturb(vector: np.ndarray, p: int) -> Union[np.ndarray, None]:
    """This function perturbs a vector by 2^p degrees and within 2^p % of its norm.
    It does so by randomly adding or subtracting 1 from a random element of the vector
    until the perturbation is achieved.

    :param vector: the vector to be perturbed
    :param p: the power of 2 to perturb the vector by
    :return: the perturbed vector
    """
    # maybe randomly add or subtract 1 until we get to 2^p degrees or 2^p % of the norm difference
    assert isinstance(vector, np.ndarray), "vector should be a numpy array"
    assert len(vector.shape) == 1, "vector should be 1-D"

    new_vector = vector.copy()
    prev_vector = None

    while angle_diff(new_vector, vector) < 2 ** p and norm_diff(new_vector, vector) < 2 ** p:
        prev_vector = new_vector.copy()
        new_vector[np.random.randint(new_vector.shape[0])] += np.random.choice([-1, 1])

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


def main(instances_fldr, samples, perturbations):
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
    assert isinstance(samples, int) and samples > 0, "samples should be a positive integer"
    assert all(isinstance(x, int) for x in perturbations), "perturbations should be a list of integers"
    assert not os.path.exists(os.path.join('test_sets', instances_fldr)),\
        f'"test_sets/{instances_fldr}" directory should not exist'
    os.mkdir(os.path.join('test_sets', instances_fldr))

    # iterate over all files in the instances directory
    for instance_file in os.listdir(os.path.join('instances', instances_fldr)):

        # get information about the instance location
        instance_name, extension = os.path.splitext(instance_file)
        # where is the original instance
        instance_pth = os.path.join('instances', instances_fldr, instance_file)
        # in what directory should the perturbed instances be stored
        perturbed_instance_dir = os.path.join('test_sets', instances_fldr, instance_name)
        # filter out non .mps files
        if extension != '.mps':
            continue
        # make a directory for each of the series for this instance
        os.mkdir(perturbed_instance_dir)
        # read in the instance
        mdl = gp.read(instance_pth)
        mdl.setParam("SolutionLimit", 1)
        mdl.setParam("TimeLimit", 1)
        # mdl.setParam('OutputFlag', 0)

        for p in perturbations:
            count = {"objective": 1, "rhs": 1, "matrix": 1, "bound": 1, "all": 1}
            for kind in count:

                # create a directory for each kind of perturbation of this instance
                series_fldr = os.path.join(perturbed_instance_dir, f"{kind}_{p}")
                os.mkdir(series_fldr)
                # Copy and rename the file as the first instance
                shutil.copy(instance_pth, os.path.join(series_fldr, f"{instance_name}_0{extension}"))

            iterations = 0
            # make a bunch of random perturbations of the instance until hopefully we get <sample> feasible ones
            while iterations < samples:
                iterations += 1

                # perturb the instance
                coefs = perturb(np.array(mdl.getA().data), p)
                A = csr_matrix((coefs, mdl.getA().nonzero()), mdl.getA().shape) \
                    if coefs is not None else None
                b = perturb(np.array(mdl.getAttr('RHS')), p)
                c = perturb(np.array(mdl.getAttr('OBJ')), p)
                l, u = perturb_bounds(mdl, p)

                # write the objective perturbation if it is feasible
                if c is not None and count["objective"] <= samples:
                    tmp_mdl = mdl.copy()
                    for j, coef in enumerate(c):
                        tmp_mdl.getVars()[j].Obj = coef
                    tmp_mdl.setParam("SolutionLimit", 1)
                    tmp_mdl.optimize()
                    if tmp_mdl.solCount >= 1:
                        tmp_mdl.write(os.path.join(perturbed_instance_dir, f"objective_{p}",
                                                   f"{instance_name}_{count['objective']}{extension}"))
                        count["objective"] += 1

                # write the rhs perturbation if it is feasible
                if b is not None and count["rhs"] <= samples:
                    tmp_mdl = mdl.copy()
                    tmp_mdl.setParam("SolutionLimit", 1)
                    for i, coef in enumerate(b):
                        tmp_mdl.getConstrs()[i].Rhs = coef
                    tmp_mdl.optimize()
                    if tmp_mdl.solCount >= 1:
                        tmp_mdl.write(os.path.join(perturbed_instance_dir, f"rhs_{p}",
                                                   f"{instance_name}_{count['rhs']}{extension}"))
                        count['rhs'] += 1

                # write the matrix perturbation if it is feasible
                if A is not None and count["matrix"] <= samples:
                    tmp_mdl = mdl.copy()
                    tmp_mdl.setParam("SolutionLimit", 1)
                    for i, j in zip(*A.nonzero()):
                        tmp_mdl.chgCoeff(tmp_mdl.getConstrs()[i], tmp_mdl.getVars()[j], A[i, j])
                    tmp_mdl.optimize()
                    if tmp_mdl.solCount >= 1:
                        tmp_mdl.write(os.path.join(perturbed_instance_dir, f"matrix_{p}",
                                                   f"{instance_name}_{count['matrix']}{extension}"))
                        count['matrix'] += 1

                # write the bound perturbation if it is feasible
                if l is not None and u is not None and count["bound"] <= samples:
                    tmp_mdl = mdl.copy()
                    tmp_mdl.setParam("SolutionLimit", 1)
                    for j, (lb, ub) in enumerate(zip(l, u)):
                        tmp_mdl.getVars()[j].lb = lb
                        tmp_mdl.getVars()[j].ub = ub
                    tmp_mdl.optimize()
                    if tmp_mdl.solCount >= 1:
                        tmp_mdl.write(os.path.join(perturbed_instance_dir, f"bound_{p}",
                                                   f"{instance_name}_{count['bound']}{extension}"))
                        count['bound'] += 1

                # write the all perturbation if it is feasible
                if l is not None and u is not None and A is not None and \
                        b is not None and c is not None and count["all"] <= samples:
                    tmp_mdl = mdl.copy()
                    tmp_mdl.setParam("SolutionLimit", 1)
                    for j, coef in enumerate(c):
                        tmp_mdl.getVars()[j].Obj = coef
                        tmp_mdl.getVars()[j].lb = l[j]
                        tmp_mdl.getVars()[j].ub = u[j]
                    for i, coef in enumerate(b):
                        tmp_mdl.getConstrs()[i].Rhs = coef
                    for i, j in zip(*A.nonzero()):
                        tmp_mdl.chgCoeff(tmp_mdl.getConstrs()[i], tmp_mdl.getVars()[j], A[i, j])
                    tmp_mdl.optimize()
                    if tmp_mdl.solCount >= 1:
                        tmp_mdl.write(os.path.join(perturbed_instance_dir, f"all_{p}",
                                                   f"{instance_name}_{count['all']}{extension}"))
                        count['all'] += 1

            for kind, amount in count.items():
                if amount <= samples:
                    print(f"Warning: {instance_name} has {amount - 1} {kind} for p = {p}")
                if amount == 1:
                    # delete the folder if there are no perturbations
                    shutil.rmtree(os.path.join(perturbed_instance_dir, f"{kind}_{p}"))

            if all(amount == 1 for amount in count.values()):
                break


if __name__ == '__main__':
    main(instances_fldr="miplib_small_easy", samples=10, perturbations=[1, 0, -1])
