from coinor.grumpy.polyhedron2D import Polyhedron2D, Figure
from matplotlib import pyplot as plt
import numpy as np

plt.rcParams['font.size'] = 12

# update plt to use latex fonts
plt.rcParams['text.usetex'] = True

# # P7
# # make a first polyhedron with a disjunctive cut
# # make polyhedron
# A_1 = np.array(
#     [[1, -1],
#      [-1, -1],
#      [1, 0],
#      [0, 1]]
# )
# b_1 = np.array([0, -3, 0, 0])
# # make children terms
# A_11 = np.vstack([A_1, [-1, 0]])
# b_11 = np.append(b_1, -1)
# A_12 = np.vstack([A_1, [1, 0]])
# b_12 = np.append(b_1, 2)
# # make disjunctive cut
# alpha = [0, 1]
# beta = 1
#
# # instantiate polyhedra
# op = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
# lc = Polyhedron2D(A=-A_11, b=-b_11)
# rc = Polyhedron2D(A=-A_12, b=-b_12)
# f = Figure()
# f.add_polyhedron(op, color='red', show_int_points=True, label='$P^1$', linestyle='dotted')
# f.add_polyhedron(lc, color='red', show_int_points=True, label='$Q^{1,1}$')
# f.add_polyhedron(rc, color='red', show_int_points=True, label='$Q^{1,2}$')
# f.set_xlim([-.5, 4.5])
# f.set_ylim([-.5, 2.5])
#
# # add disjunction and disjunctive cut
# f.add_line(alpha, beta, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dashed', color='green', label=r"$x_2 \leq 1$")
# # f.ax.annotate('', xy=(1.5, 0.25), xytext=(1.5, 1),
# #               arrowprops=dict(arrowstyle='-|>', color='green', lw=1, linestyle='--'))
# f.add_line([1, 0], 1, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dotted', color='black', label="$x_1 = 1$ or $x_1 = 2$")
# f.add_line([1, 0], 2, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dotted', color='black')
# f.show(wait_for_click=False, filename='P7.png')
#
# # P8
# # make a first polyhedron with a disjunctive cut
# # make polyhedron
# A_1 = np.array(
#     [[1, -1],
#      [-1, -1],
#      [1, 0],
#      [0, 1]]
# )
# b_1 = np.array([0, -3.5, 0, 0])
# # make children terms
# A_11 = np.vstack([A_1, [-1, 0]])
# b_11 = np.append(b_1, -1)
# A_12 = np.vstack([A_1, [1, 0]])
# b_12 = np.append(b_1, 2)
# # make disjunctive cut
# alpha = [0, 1]
# beta = 1.5
#
# # instantiate polyhedra
# op = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
# lc = Polyhedron2D(A=-A_11, b=-b_11)
# rc = Polyhedron2D(A=-A_12, b=-b_12)
# f = Figure()
# f.add_polyhedron(op, color='red', show_int_points=True, label='$P^2$', linestyle='dotted')
# f.add_polyhedron(lc, color='red', show_int_points=True, label='$Q^{2,1}$')
# f.add_polyhedron(rc, color='red', show_int_points=True, label='$Q^{2,2}$')
# f.set_xlim([-.5, 4.5])
# f.set_ylim([-.5, 2.5])
#
# # add disjunction and disjunctive cut
# f.add_line(alpha, beta, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dashed', color='green', label=r"$x_2 \leq 1.5$")
# # f.ax.annotate('', xy=(1.5, 0.75), xytext=(1.5, 1.5),
# #               arrowprops=dict(arrowstyle='-|>', color='green', lw=1, linestyle='--'))
# f.add_line([1, 0], 1, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dotted', color='black', label="$x_1 = 1$ or $x_1 = 2$")
# f.add_line([1, 0], 2, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dotted', color='black')
# f.show(wait_for_click=False, filename='P8.png')
#
# # condense the last two into one
# A_1 = np.array(
#     [[1, -1],
#      [-1, -1],
#      [1, 0],
#      [0, 1]]
# )
# b_1 = np.array([0, -3, 0, 0])
# # make children terms
# A_11 = np.vstack([A_1, [-1, 0]])
# b_11 = np.append(b_1, -1)
# A_12 = np.vstack([A_1, [1, 0]])
# b_12 = np.append(b_1, 2)
# # make disjunctive cut
# alpha_1 = [0, 1]
# beta_1 = 1
#
# # make perturbed instance
# A_2 = np.array(
#     [[1, -1],
#      [-1, -1],
#      [1, 0],
#      [0, 1]]
# )
# b_2 = np.array([0, -3.5, 0, 0])
# # make children terms
# A_21 = np.vstack([A_2, [-1, 0]])
# b_21 = np.append(b_2, -1)
# A_22 = np.vstack([A_2, [1, 0]])
# b_22 = np.append(b_2, 2)
# # make disjunctive cut
# alpha_2 = [0, 1]
# beta_2 = 1.5
#
# # instantiate polyhedra
# op_1 = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
# lc_1 = Polyhedron2D(A=-A_11, b=-b_11)
# rc_1 = Polyhedron2D(A=-A_12, b=-b_12)
# op_2 = Polyhedron2D(A=-A_2, b=-b_2)
# lc_2 = Polyhedron2D(A=-A_21, b=-b_21)
# rc_2 = Polyhedron2D(A=-A_22, b=-b_22)
# f = Figure()
# f.add_polyhedron(op_1, color='red', show_int_points=False, linestyle='dotted')
# f.add_polyhedron(op_2, color='red', show_int_points=False, linestyle='solid')
# f.set_xlim([-.25, 3.75])
# f.set_ylim([-.125, 1.875])
#
# # add disjunction and disjunctive cut
# f.add_line(alpha_1, beta_1, op_1.xlim + [0, 0.8], op_1.ylim + [0.2, 1.8],
#            linestyle=':', color='green')
# f.add_line(alpha_2, beta_2, op_1.xlim + [0, 0.8], op_1.ylim + [0.2, 1.8],
#            linestyle='--', color='green')
# # f.add_line([1, 0], 1, op_2.xlim + [0, 0.8], op_2.ylim + [0.2, 1.8],
# #            linestyle='dotted', color='black')
# # f.add_line([1, 0], 2, op_2.xlim + [0, 0.8], op_2.ylim + [0.2, 1.8],
# #            linestyle='dotted', color='black')
#
# # shade in feasible regions
# f.ax.fill(lc_1.hrep.generators[:, 0], lc_1.hrep.generators[:, 1],
#           color='red', alpha=0.3)
# f.ax.fill(lc_2.hrep.generators[:, 0], lc_2.hrep.generators[:, 1],
#           color='red', alpha=0.2)
# f.ax.fill(rc_1.hrep.generators[:, 0], rc_1.hrep.generators[:, 1],
#           color='red', alpha=0.3)
# f.ax.fill(rc_2.hrep.generators[:, 0], rc_2.hrep.generators[:, 1],
#           color='red', alpha=0.2)
#
# # annotate
# f.ax.annotate('', xy=(.6, 1), xytext=(.5, .75),
#               arrowprops=dict(arrowstyle='-|>', color='black', lw=1))
# f.ax.text(-.15, .75, "1) generate\ndisjunctive cut", ha='left', va='center', fontsize=12, color='black')
# f.ax.annotate('', xy=(2.75, .75), xytext=(2.5, .5),
#               arrowprops=dict(arrowstyle='-|>', color='black', lw=1))
# f.ax.text(2.65, .575, "2) perturb", ha='left', va='center', fontsize=12, color='black')
# f.ax.annotate('', xy=(2.25, 1.5), xytext=(2.25, 1),
#               arrowprops=dict(arrowstyle='-|>', color='black', lw=1))
# f.ax.text(2.3, 1.3, "3) parameterize", ha='left', va='center', fontsize=12, color='black')
#
# f.show(wait_for_click=False, filename='P7-8.png')

#
# # P9
# # make a matrix perturbed polyhedron with a parametrized disjunctive cut
# # make polyhedron and children terms
# A_1 = np.array(
#     [[1.1, -1],
#      [-1, -0.9],
#      [1, 0],
#      [0, 1]]
# )
# b_1 = np.array([0, -3, 0, 0])
# # make children terms
# A_11 = np.vstack([A_1, [-1, 0]])
# b_11 = np.append(b_1, -1)
# A_12 = np.vstack([A_1, [1, 0]])
# b_12 = np.append(b_1, 2)
# # make disjunctive cut
# alpha = [1, -9]
# beta = -10
#
# # instantiate polyhedra
# op = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
# lc = Polyhedron2D(A=-A_11, b=-b_11)
# rc = Polyhedron2D(A=-A_12, b=-b_12)
# f = Figure()
# f.add_polyhedron(op, color='red', show_int_points=True, label='$P^3$', linestyle='dotted')
# f.add_polyhedron(lc, color='red', show_int_points=True, label='$Q^{3,1}$')
# f.add_polyhedron(rc, color='red', show_int_points=True, label='$Q^{3,2}$')
# f.set_xlim([-.5, 4.5])
# f.set_ylim([-.5, 2.5])
#
# # add disjunction and disjunctive cut
# f.add_line(alpha, beta, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dashed', color='green', label=r"$x_1 - 9x_2 \geq -10$")
# # f.ax.annotate('', xy=(1.5, .6), xytext=(1.4, 1.25),
# #               arrowprops=dict(arrowstyle='-|>', color='green', lw=1, linestyle='--'))
# f.add_line([1, 0], 1, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dashed', color='black', label="$x_1 = 1$ or $x_1 = 2$")
# f.add_line([1, 0], 2, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dashed', color='black')
# f.show(wait_for_click=False, filename='P9.png')
#
# # P9.1
# # make a matrix perturbed polyhedron with a parametrized disjunctive cut
# # make polyhedron and children terms  they're equal at A_1_00 = 2 and A_1_11 = -.5
# A_1 = np.array(
#     [[3, -1],
#      [-1, -0.25],
#      [1, 0],
#      [0, 1]]
# )
# b_1 = np.array([0, -3, 0, 0])
# # make children terms
# A_11 = np.vstack([A_1, [-1, 0]])
# b_11 = np.append(b_1, -1)
# A_12 = np.vstack([A_1, [1, 0]])
# b_12 = np.append(b_1, 2)
# # make disjunctive cut
# v_1 = np.array([1, 0, 0, 0, 1])
# v_2 = np.array([0, 1, 0, 0, 1])
# alpha = np.maximum(v_1 @ A_11, v_2 @ A_12)
# beta = np.min(np.dot(v_1, b_11), np.dot(v_2, b_12))
#
# # instantiate polyhedra
# op = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
# lc = Polyhedron2D(A=-A_11, b=-b_11)
# rc = Polyhedron2D(A=-A_12, b=-b_12)
# f = Figure()
# f.add_polyhedron(op, color='red', show_int_points=True, label='$P^3$', linestyle='dotted')
# f.add_polyhedron(lc, color='red', show_int_points=True, label='$Q^{3,1}$')
# f.add_polyhedron(rc, color='red', show_int_points=True, label='$Q^{3,2}$')
# f.set_xlim([-.5, 4.5])
# f.set_ylim([-.5, 6])
#
# # add disjunction and disjunctive cut
# f.add_line(alpha, beta, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dashed', color='green',
#            label=f"$[{round(alpha[0], 2)}, {round(alpha[1], 2)}]^T x \geq {round(beta, 2)}$")
# # f.ax.annotate('', xy=(1.5, .6), xytext=(1.4, 1.25),
# #               arrowprops=dict(arrowstyle='-|>', color='green', lw=1, linestyle='--'))
# f.add_line([1, 0], 1, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dashed', color='black', label="$x_1 = 1$ or $x_1 = 2$")
# f.add_line([1, 0], 2, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dashed', color='black')
# f.show(wait_for_click=False, filename='P91.png')
#
# # P10
# # make tightened disjunctive cut
# A_1 = np.array(
#     [[1.1, -1],
#      [-1, -0.9],
#      [1, 0],
#      [0, 1]]
# )
# b_1 = np.array([0, -3, 0, 0])
# # make children terms
# A_11 = np.vstack([A_1, [-1, 0]])
# b_11 = np.append(b_1, -1)
# A_12 = np.vstack([A_1, [1, 0]])
# b_12 = np.append(b_1, 2)
# alpha = [1, -9]
# beta = -8.9
#
# # instantiate polyhedra
# op = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
# lc = Polyhedron2D(A=-A_11, b=-b_11)
# rc = Polyhedron2D(A=-A_12, b=-b_12)
# f = Figure()
# f.add_polyhedron(op, color='red', show_int_points=True, label='$P^3$', linestyle='dotted')
# f.add_polyhedron(lc, color='red', show_int_points=True, label='$Q^{3,1}$')
# f.add_polyhedron(rc, color='red', show_int_points=True, label='$Q^{3,2}$')
# f.set_xlim([-.5, 4.5])
# f.set_ylim([-.5, 2.5])
#
# # add disjunction and disjunctive cut
# f.add_line(alpha, beta, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dashed', color='green', label=r"$x_1 - 9x_2 \geq -8.9$")
# # f.ax.annotate('', xy=(1.5, .5), xytext=(1.4, 1.15),
# #               arrowprops=dict(arrowstyle='-|>', color='green', lw=1, linestyle='--'))
# f.add_line([1, 0], 1, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dashed', color='black', label="$x_1 = 1$ or $x_1 = 2$")
# f.add_line([1, 0], 2, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dashed', color='black')
# f.show(wait_for_click=False, filename='P10.png')

if False:

    # P9-10
    # make a matrix perturbed polyhedron with a matrix parametrized disjunctive cut
    A_1 = np.array(
        [[1, -1],
         [-1, -1],
         [1, 0],
         [0, 1]]
    )
    b_1 = np.array([0, -3, 0, 0])
    # make children terms
    A_11 = np.vstack([A_1, [-1, 0]])
    b_11 = np.append(b_1, -1)
    A_12 = np.vstack([A_1, [1, 0]])
    b_12 = np.append(b_1, 2)
    # make disjunctive cut
    alpha_1 = [0, 1]
    beta_1 = 1

    # make perturbed instance
    A_2 = np.array(
        [[1.1, -1],
         [-1, -0.9],
         [1, 0],
         [0, 1]]
    )
    b_2 = np.array([0, -3, 0, 0])
    # make children terms
    A_21 = np.vstack([A_2, [-1, 0]])
    b_21 = np.append(b_2, -1)
    A_22 = np.vstack([A_2, [1, 0]])
    b_22 = np.append(b_2, 2)
    # make disjunctive cut
    alpha_2 = [1, -9]
    beta_2 = -10
    beta_2_tight = -8.9

    # instantiate polyhedra
    op_1 = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
    lc_1 = Polyhedron2D(A=-A_11, b=-b_11)
    rc_1 = Polyhedron2D(A=-A_12, b=-b_12)
    op_2 = Polyhedron2D(A=-A_2, b=-b_2)
    lc_2 = Polyhedron2D(A=-A_21, b=-b_21)
    rc_2 = Polyhedron2D(A=-A_22, b=-b_22)
    f = Figure()
    f.add_polyhedron(op_1, color='red', show_int_points=False, linestyle='dotted')
    f.add_polyhedron(op_2, color='red', show_int_points=False, linestyle='solid')
    f.set_xlim([-.25, 3.25])
    f.set_ylim([-.25, 1.85])

    # add disjunction and disjunctive cut
    f.add_line(alpha_1, beta_1, op_1.xlim + [0, 0.8], op_1.ylim + [0.2, 1.8],
               linestyle=':', color='green')
    f.add_line(alpha_2, beta_2, op_1.xlim + [0, 0.8], op_1.ylim + [0.2, 1.8],
               linestyle='-.', color='green')
    f.add_line(alpha_2, beta_2_tight, op_1.xlim + [0, 0.8], op_1.ylim + [0.2, 1.8],
               linestyle='--', color='green')
    # f.add_line([1, 0], 1, op_2.xlim + [0, 0.8], op_2.ylim + [0.2, 1.8],
    #            linestyle='dotted', color='black')
    # f.add_line([1, 0], 2, op_2.xlim + [0, 0.8], op_2.ylim + [0.2, 1.8],
    #            linestyle='dotted', color='black')

    # shade in feasible regions
    f.ax.fill(lc_1.hrep.generators[:, 0], lc_1.hrep.generators[:, 1],
              color='red', alpha=0.3)
    f.ax.fill(lc_2.hrep.generators[:, 0], lc_2.hrep.generators[:, 1],
              color='red', alpha=0.2)
    f.ax.fill(rc_1.hrep.generators[:, 0], rc_1.hrep.generators[:, 1],
              color='red', alpha=0.3)
    f.ax.fill(rc_2.hrep.generators[:, 0], rc_2.hrep.generators[:, 1],
              color='red', alpha=0.2)

    # annotate
    f.ax.annotate('', xy=(.75, .875), xytext=(.75, .7),
                  arrowprops=dict(arrowstyle='-|>', color='black', lw=1))
    f.ax.text(.45, .775, "1) perturb", ha='center', va='center', fontsize=12, color='black')
    f.ax.annotate('', xy=(.8, 1.2), xytext=(.85, 1),
                  arrowprops=dict(arrowstyle='-|>', color='black', lw=1))
    f.ax.text(.4, 1.23, "2) parameterize", ha='center', va='center', fontsize=12, color='black')
    f.ax.annotate('', xy=(2.2 + 1*2.5/64, 1.37 - 4*2.5/64), xytext=(2.2, 1.37),
                  arrowprops=dict(arrowstyle='-|>', color='black', lw=1))
    f.ax.text(2.125, 1.125, "3) tighten", ha='left', va='center', fontsize=12, color='black')
    f.ax.text(2.39, .75, "coefficient matrix\nchanged", ha='left', va='center', fontsize=12, color='black')
    f.ax.annotate('', xy=(2.15, .875), xytext=(2.39, .8),
                  arrowprops=dict(arrowstyle='-', color='black', lw=1))

    f.show(wait_for_click=False, filename='P9-10.png')
#
# # P9-10_simple
# # simplify the above to just show the perturbation case
# f = Figure()
# f.add_polyhedron(op_1, color='red', show_int_points=False, linestyle='dotted')
# f.add_polyhedron(op_2, color='red', show_int_points=False, linestyle='solid')
# f.set_xlim([-.25, 3.25])
# f.set_ylim([-.25, 1.85])
#
# # add disjunction and disjunctive cut
# f.add_line(alpha_1, beta_1, op_1.xlim + [0, 0.8], op_1.ylim + [0.2, 1.8],
#            linestyle=':', color='green')
# f.add_line(alpha_2, beta_2, op_1.xlim + [0, 0.8], op_1.ylim + [0.2, 1.8],
#            linestyle='--', color='green')
#
# # shade in feasible regions
# f.ax.fill(lc_1.hrep.generators[:, 0], lc_1.hrep.generators[:, 1],
#           color='red', alpha=0.3)
# f.ax.fill(lc_2.hrep.generators[:, 0], lc_2.hrep.generators[:, 1],
#           color='red', alpha=0.2)
# f.ax.fill(rc_1.hrep.generators[:, 0], rc_1.hrep.generators[:, 1],
#           color='red', alpha=0.3)
# f.ax.fill(rc_2.hrep.generators[:, 0], rc_2.hrep.generators[:, 1],
#           color='red', alpha=0.2)
#
# # annotate
# f.ax.annotate('', xy=(.6, 1), xytext=(.5, .75),
#               arrowprops=dict(arrowstyle='-|>', color='black', lw=1))
# f.ax.text(-.15, .75, "1) generate\ndisjunctive cut", ha='left', va='center', fontsize=12, color='black')
# f.ax.annotate('', xy=(2.25, .875), xytext=(2.25, .7),
#               arrowprops=dict(arrowstyle='-|>', color='black', lw=1))
# f.ax.text(2.36, .75, "2) perturb\nmatrix coefficients", ha='left', va='center', fontsize=12, color='black')
# f.ax.annotate('', xy=(2.13, 1.36), xytext=(2.25, 1),
#               arrowprops=dict(arrowstyle='-|>', color='black', lw=1))
# f.ax.text(2.6, 1.23, "3) parameterize", ha='center', va='center', fontsize=12, color='black')
#
# f.show(wait_for_click=False, filename='P9-10_simple.png')

#
# # P11
# # make polyhedra
# A_1 = np.array(
#     [[1, -1],
#      [-1, -1],
#      [1, 0],
#      [0, 1]]
# )
# b_1 = np.array([0, -3, 0, 0])
# C_11 = np.array([
#     [1, -1],
#     [-1, 0]
# ])
# C0_11 = np.array([0, -1])
# C_12 = np.array([
#     [-1, -1],
#     [1, 0]
# ])
# C0_12 = np.array([-3, 2])
# # make disjunctive cut
# alpha = [0, 1]
# beta = 1
# Alpha = np.array([alpha])
# Alpha0 = np.array([beta])
#
# # instantiate polyhedra
# op = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
# lc = Polyhedron2D(A=-C_11, b=-C0_11)
# rc = Polyhedron2D(A=-C_12, b=-C0_12)
# cut = Polyhedron2D(A=-Alpha, b=-Alpha0)
# f = Figure()
# f.add_polyhedron(op, color='red', show_int_points=True, label='$P^1$', linestyle='dotted')
# # todo: change self.ax.arrow 3rd and 4th arguments in add_polyhedron to ray[0]/(ray[0]**2 + ray[1]**2)**.5, ray[1]/(ray[0]**2 + ray[1]**2)**.5
# f.add_polyhedron(lc, color='red', show_int_points=True, label='$\mathcal{R}^{1, 1} \cup \mathcal{R}^{1, 2}$')
# f.add_polyhedron(rc, color='red', show_int_points=True)
# f.set_xlim([-.5, 4.5])
# f.set_ylim([-.5, 2.5])
#
# # add local optima
# f.ax.plot(1, 1, 'ko', markersize=4, label=r'$\mathcal{E}^{1, 1} \cup \mathcal{E}^{1, 2}$')
# f.ax.plot(2, 1, 'ko', markersize=4)
#
# # add disjunction and disjunctive cut
# f.add_line(alpha, beta, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dashed', color='green', label=r"$x_2 = 1$")
# f.add_line([1, 0], 1, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dotted', color='black', label="$x_1 = 1$ or $x_1 = 2$")
# f.add_line([1, 0], 2, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dotted', color='black')
#
# # objective direction
# f.ax.annotate('', xy=(1.5, 1), xytext=(1.5, 1.5),
#               arrowprops=dict(arrowstyle='-|>', color='blue', lw=1))
# f.add_line(alpha, 10, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            color='blue', label="$w$")
# # f.ax.annotate('', xy=(0.25, 1.25), xytext=(1, 1.25),
# #               arrowprops=dict(arrowstyle='-|>', color='black', lw=1, linestyle=':'))
# # f.ax.annotate('', xy=(2.75, 1.25), xytext=(2, 1.25),
# #               arrowprops=dict(arrowstyle='-|>', color='black', lw=1, linestyle=':'))
# f.show(wait_for_click=False, filename='P11.png')
#
# # P12
# # make a polyhedron again with a parametric disjunctive cut
# # make polyhedron
# A_1 = np.array(
#     [[1, -1],
#      [-1, -1],
#      [1, 0],
#      [0, 1]]
# )
# b_1 = np.array([0, -3, 0, 0])
# # make children terms
# A_11 = np.vstack([A_1, [-1, 0]])
# b_11 = np.append(b_1, -1)
# A_12 = np.vstack([A_1, [1, 0]])
# b_12 = np.append(b_1, 2)
# # make disjunctive cut
# alpha = [1, -3]
# beta = -2
#
# # instantiate polyhedra
# op = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
# lc = Polyhedron2D(A=-A_11, b=-b_11)
# rc = Polyhedron2D(A=-A_12, b=-b_12)
# f = Figure()
# f.add_polyhedron(op, color='red', show_int_points=False, label='$P^1$', linestyle='dotted')
# f.add_polyhedron(lc, color='red', show_int_points=False, label='$Q^{1,1} \cup Q^{1,2}$')
# f.add_polyhedron(rc, color='red', show_int_points=False) #, label='$Q^{1,2}$')
# f.set_xlim([-.5, 4.5])
# f.set_ylim([-.5, 2.5])
#
# # add incumbent solution
# f.ax.plot(2, 1, 'ko', markersize=4, label=r'$x^* = (2, 1)$')
#
# # add disjunction and disjunctive cut
# f.add_line(alpha, beta, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dashed', color='green', label=r"$x_1 - 3x_2 \leq -2$")
# # f.ax.annotate('', xy=(1.5, 0.25), xytext=(1.5, 1),
# #               arrowprops=dict(arrowstyle='-|>', color='green', lw=1, linestyle='--'))
# f.add_line([1, 0], 1, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dotted', color='black', label="$x_1 = 1$ or $x_1 = 2$")
# f.add_line([1, 0], 2, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dotted', color='black')
#
# # objective direction
# f.ax.annotate('', xy=(1.5, 2), xytext=(1.5, 1.5),
#               arrowprops=dict(arrowstyle='-|>', color='blue', lw=1))
# f.add_line([1, 0], 1.5, op.xlim, [1.5, 1.9],
#            color='blue', label=r"$c^1$")
#
# # set legend to appear center-right
# f.ax.legend(loc='center right')
# f.show(wait_for_click=False, filename='P12.png')
#
# # P13
# # make a polyhedron again with a parametric disjunctive cut
# # make polyhedron
# A_1 = np.array(
#     [[1, -1],
#      [-1, -1],
#      [1, 0],
#      [0, 1]]
# )
# b_1 = np.array([0, -3, 0, 0])
# # make children terms
# A_11 = np.vstack([A_1, [-1, 0]])
# b_11 = np.append(b_1, -1)
# A_12 = np.vstack([A_1, [1, 0]])
# b_12 = np.append(b_1, 2)
# # make disjunctive cut
# alpha = [1, -3]
# beta = -2
#
# # instantiate polyhedra
# op = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
# lc = Polyhedron2D(A=-A_11, b=-b_11)
# rc = Polyhedron2D(A=-A_12, b=-b_12)
# f = Figure()
# f.add_polyhedron(op, color='red', show_int_points=False, label='$P^4$', linestyle='dotted')
# f.add_polyhedron(lc, color='red', show_int_points=False, label='$Q^{4,1} \cup Q^{4,2}$')
# f.add_polyhedron(rc, color='red', show_int_points=False) #, label='$Q^{1,2}$')
# f.set_xlim([-.5, 4.5])
# f.set_ylim([-.5, 2.5])
#
# # add incumbent solution
# f.ax.plot(2, 1, 'ko', markersize=4, label=r'$x^* = (2, 1)$')
#
# # add disjunction and disjunctive cut
# f.add_line(alpha, beta, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dashed', color='green', label=r"$x_1 - 3x_2 \leq -2$")
# # f.ax.annotate('', xy=(1.5, 0.25), xytext=(1.5, 1),
# #               arrowprops=dict(arrowstyle='-|>', color='green', lw=1, linestyle='--'))
# f.add_line([1, 0], 1, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dotted', color='black', label="$x_1 = 1$ or $x_1 = 2$")
# f.add_line([1, 0], 2, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dotted', color='black')
#
# # objective direction
# f.ax.annotate('', xy=(1.75, 2), xytext=(1.5, 1.5),
#               arrowprops=dict(arrowstyle='-|>', color='blue', lw=1))
# f.add_line([2, -1], 1.5, [1.5, 1.7], [1.5, 1.9],
#            color='blue', label=r"$c^4$")
#
# # set legend to appear center-right
# f.ax.legend(loc='center right')
# f.show(wait_for_click=False, filename='P13.png')
#
# # P14
# # make the polyhedron again but with a parametric disjunctive optimality cut
# # make polyhedron
# A_1 = np.array(
#     [[1, -1],
#      [-1, -1],
#      [1, 0],
#      [0, 1]]
# )
# b_1 = np.array([0, -3, 0, 0])
# # make children terms
# A_11 = np.vstack([A_1, [-1, 0]])
# b_11 = np.append(b_1, -1)
# A_12 = np.vstack([A_1, [1, 0]])
# b_12 = np.append(b_1, 2)
# # make disjunctive cut
# alpha = [1, -3]
# beta = -1
#
# # instantiate polyhedra
# op = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
# lc = Polyhedron2D(A=-A_11, b=-b_11)
# rc = Polyhedron2D(A=-A_12, b=-b_12)
# f = Figure()
# f.add_polyhedron(op, color='red', show_int_points=False, label='$P^4$', linestyle='dotted')
# # f.add_polyhedron(lc, color='red', show_int_points=False, label='$Q^{1,1}$')
# f.add_polyhedron(rc, color='red', show_int_points=False, label='$Q^{4,2}$')
# f.set_xlim([-.5, 4.5])
# f.set_ylim([-.5, 2.5])
#
# # add incumbent solution
# f.ax.plot(2, 1, 'ko', markersize=4, label=r'$x^* = (2, 1)$')
#
# # add disjunction and disjunctive cut
# f.add_line(alpha, beta, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dashed', color='green', label=r"$x_1 - 3x_2 \leq -1$")
# # f.ax.annotate('', xy=(1.5, 0.25), xytext=(1.5, 1),
# #               arrowprops=dict(arrowstyle='-|>', color='green', lw=1, linestyle='--'))
# f.add_line([1, 0], 1, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dotted', color='black', label="$x_1 = 1$ or $x_1 = 2$")
# f.add_line([1, 0], 2, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
#            linestyle='dotted', color='black')
#
# # objective direction
# f.ax.annotate('', xy=(1.75, 2), xytext=(1.5, 1.5),
#               arrowprops=dict(arrowstyle='-|>', color='blue', lw=1))
# f.add_line([2, -1], 1.5, [1.5, 1.7], [1.5, 1.9],
#            color='blue', label=r"$c^4$")
#
# # set legend to appear center-right
# f.ax.legend(loc='center right')
# f.show(wait_for_click=False, filename='P14.png')

if False:

    # P13-14
    # combine last two into one
    A_1 = np.array(
        [[3, -1],
         [-1, -1],
         [1, 0],
         [0, 1]]
    )
    b_1 = np.array([2.5, -3, 0, 0])
    A_11 = np.vstack([A_1, [-1, 0]])
    b_11 = np.append(b_1, -1)
    A_12 = np.vstack([A_1, [1, 0]])
    b_12 = np.append(b_1, 2)
    # make disjunctive cut
    alpha_1 = [1, -2]
    beta_1 = 0

    # make polyhedron
    A_2 = np.array(
        [[3, -1],
         [-1, -1],
         [1, 0],
         [0, 1]]
    )
    b_2 = np.array([2, -3.125, 0, 0])
    A_21 = np.vstack([A_2, [-1, 0]])
    b_21 = np.append(b_2, -1)
    A_22 = np.vstack([A_2, [1, 0]])
    b_22 = np.append(b_2, 2)
    # make disjunctive cut
    alpha_2 = [1, -2]
    beta_2 = -1
    beta_2_tight = -1/4

    # instantiate polyhedra
    op_1 = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
    lc_1 = Polyhedron2D(A=-A_11, b=-b_11)
    rc_1 = Polyhedron2D(A=-A_12, b=-b_12)
    op_2 = Polyhedron2D(A=-A_2, b=-b_2)
    lc_2 = Polyhedron2D(A=-A_21, b=-b_21)
    rc_2 = Polyhedron2D(A=-A_22, b=-b_22)
    f = Figure()
    f.add_polyhedron(op_1, color='red', show_int_points=False, linestyle='dotted')
    f.add_polyhedron(op_2, color='red', show_int_points=False, linestyle='-')
    f.set_xlim([.5, 3.25])
    f.set_ylim([-.25, 2.25])

    # add disjunction and disjunctive cut
    f.add_line(alpha_1, beta_1, op_2.xlim + [0, 0.8], op_2.ylim + [0.2, 1.8],
               linestyle=':', color='green')
    f.add_line(alpha_2, beta_2, op_2.xlim + [0, 0.8], op_2.ylim + [0.2, 1.8],
               linestyle='-.', color='green', alpha=.5)
    f.add_line(alpha_2, beta_2_tight, op_2.xlim + [0, 0.8], op_2.ylim + [0.2, 1.8],
               linestyle='--', color='green')
    # f.add_line([1, 0], 1, op_2.xlim + [0, 0.8], op_2.ylim + [0.2, 1.8],
    #            linestyle='dotted', color='black')
    # f.add_line([1, 0], 2, op_2.xlim + [0, 0.8], op_2.ylim + [0.2, 1.8],
    #            linestyle='dotted', color='black')

    # primal bound stuff
    scale = .35
    f.ax.annotate('', xy=(1.28 + scale/10, 1.83 + scale), xytext=(1.28, 1.83),
                  arrowprops=dict(arrowstyle='-|>', color='blue', lw=1))
    f.ax.text(1.4, 2, "objective:\nmax x + 10y", ha='left', va='center', fontsize=12, color='black')
    f.ax.plot(2, 1, 'ko', markersize=4)
    f.ax.text(2.05, .95, "incumbent solution: (2, 1)", ha='left', va='center', fontsize=12, color='black')

    # shade in feasible regions
    f.ax.fill(lc_1.hrep.generators[:, 0], lc_1.hrep.generators[:, 1],
              color='red', alpha=0.3)
    f.ax.fill(lc_2.hrep.generators[:, 0], lc_2.hrep.generators[:, 1],
              color='red', alpha=0.2)
    f.ax.fill(rc_1.hrep.generators[:, 0], rc_1.hrep.generators[:, 1],
              color='red', alpha=0.3)
    f.ax.fill(rc_2.hrep.generators[:, 0], rc_2.hrep.generators[:, 1],
              color='red', alpha=0.2)


    # annotate
    f.ax.annotate('', xy=(.65, -.05), xytext=(.85, -.05),
                  arrowprops=dict(arrowstyle='-|>', color='black', lw=1))
    f.ax.text(0.75, -0.08, "1) perturb", ha='center', va='top', fontsize=12, color='black')
    f.ax.text(0.9, .35, "X", ha='center', va='center', fontsize=96, color='black')
    f.ax.text(1.02, .45, "2) prune", ha='left', va='top', fontsize=12, color='black')
    f.ax.annotate('', xy=(2.5 - 1*4/64, 1.25 + 2*4/64), xytext=(2.5, 1.25),
                  arrowprops=dict(arrowstyle='-|>', color='black', lw=1))
    f.ax.text(2.5, 1.25, "3) parameterize", ha='left', va='top', fontsize=12, color='black')
    f.ax.text(2.11, 2.1, "parameterizing\nwithout pruning", ha='left', va='center', fontsize=12, color='black')
    f.ax.annotate('', xy=(2.85, 1.95), xytext=(2.7, 2.1),
                  arrowprops=dict(arrowstyle='-', color='black', lw=1,
                                  connectionstyle="arc3,rad=-.2"))

    f.show(wait_for_click=False, filename='P13-14.png')

# make a counter example for when LP solution no longer separated
if True:
    A_1 = np.array(
        [[1, -1],
         [-1, -1],
         [0, -1],
         [1, 0],
         [0, 1]]
    )
    b_1 = np.array([0, -3, -1.4, 0, 0])
    # make children terms
    A_11 = np.vstack([A_1, [-1, 0]])
    b_11 = np.append(b_1, -1)
    A_12 = np.vstack([A_1, [1, 0]])
    b_12 = np.append(b_1, 2)
    # make disjunctive cut
    alpha_1 = [1/3, -1]
    beta_1 = -2/3

    # make perturbed instance
    A_2 = np.array(
        [[1, -1],
         [-1, -1],
         [0, -1],
         [1, 0],
         [0, 1]]
    )
    b_2 = np.array([-.25, -3.25, -1.4, 0, 0])
    # make children terms
    A_21 = np.vstack([A_2, [-1, 0]])
    b_21 = np.append(b_2, -1)
    A_22 = np.vstack([A_2, [1, 0]])
    b_22 = np.append(b_2, 2)
    # make disjunctive cut
    alpha_2 = [1/3, -1]
    beta_2 = 1/3 - 1.25

    # instantiate polyhedra
    op_1 = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
    lc_1 = Polyhedron2D(A=-A_11, b=-b_11)
    rc_1 = Polyhedron2D(A=-A_12, b=-b_12)
    op_2 = Polyhedron2D(A=-A_2, b=-b_2)
    lc_2 = Polyhedron2D(A=-A_21, b=-b_21)
    rc_2 = Polyhedron2D(A=-A_22, b=-b_22)
    f = Figure()
    f.add_polyhedron(op_1, color='red', show_int_points=False, linestyle='dotted')
    f.add_polyhedron(op_2, color='red', show_int_points=False, linestyle='solid')
    f.set_xlim([-.25, 3.75])
    f.set_ylim([-.125, 1.875])

    # add disjunctive cut and basic solutions
    f.add_line(alpha_1, beta_1, op_1.xlim + [0, 0.8], op_1.ylim + [0.2, 1.8],
               linestyle=':', color='green')
    f.add_line(alpha_2, beta_2, op_1.xlim + [0, 0.8], op_1.ylim + [0.2, 1.8],
               linestyle='--', color='green')
    f.ax.plot(1.6, 1.4, 'ko', markersize=4)
    f.ax.text(1.55, 1.38, r"$\bar{x}$", ha='center', va='top', fontsize=12, color='black')
    f.ax.plot(1.85, 1.4, 'ko', markersize=4)
    f.ax.plot(1, 1, 'ko', markersize=4)
    f.ax.text(1.025, .9875, "$x^{t}$", ha='left', va='top', fontsize=12, color='black')
    f.ax.plot(1, 1.25, 'ko', markersize=4)

    # shade in feasible regions
    f.ax.fill(lc_1.hrep.generators[:, 0], lc_1.hrep.generators[:, 1],
              color='red', alpha=0.3)
    f.ax.fill(lc_2.hrep.generators[:, 0], lc_2.hrep.generators[:, 1],
              color='red', alpha=0.2)
    f.ax.fill(rc_1.hrep.generators[:, 0], rc_1.hrep.generators[:, 1],
              color='red', alpha=0.3)
    f.ax.fill(rc_2.hrep.generators[:, 0], rc_2.hrep.generators[:, 1],
              color='red', alpha=0.2)

    # annotate
    f.ax.annotate('', xy=(.25, .75), xytext=(.25, .575),
                  arrowprops=dict(arrowstyle='-|>', color='black', lw=1))
    f.ax.text(-.15, .5, "1) generate\ndisjunctive cut", ha='left', va='center', fontsize=12, color='black')
    f.ax.annotate('', xy=(1, 1.25), xytext=(1, 1),
                  arrowprops=dict(arrowstyle='-|>', color='black', lw=1))
    f.ax.annotate('', xy=(1.85, 1.4), xytext=(1.6, 1.4),
                  arrowprops=dict(arrowstyle='-|>', color='black', lw=1))
    f.ax.annotate('', xy=(.95, 1.1), xytext=(.25, 1.6),
                  arrowprops=dict(arrowstyle='-', color='black', lw=1))
    f.ax.annotate('', xy=(1.725, 1.425), xytext=(.6, 1.65),
                  arrowprops=dict(arrowstyle='-', color='black', lw=1))
    f.ax.text(0, 1.65, "2) perturb", ha='left', va='center', fontsize=12, color='black')
    f.ax.annotate('', xy=(2.25, 1.675), xytext=(2.25, 1.4125),
                  arrowprops=dict(arrowstyle='-|>', color='black', lw=1))
    f.ax.text(2.3, 1.375, "3) parameterize", ha='left', va='center', fontsize=12, color='black')

    f.show(wait_for_click=False, filename='P15-16.png')

if False:

    # show how infeasible terms become feasible during parameterization
    # P17-18
    # make a pair of polyhedra where a disjunctive term goes from infeasible to feasible
    A_1 = np.array(
        [[1, -1],
         [-1, -1],
         [1, 0],
         [0, 1]]
    )
    b_1 = np.array([0, -2.5, 1.25, 0])
    # make children terms
    A_11 = np.vstack([A_1, [-1, 0]])
    b_11 = np.append(b_1, -1)
    A_12 = np.vstack([A_1, [1, 0]])
    b_12 = np.append(b_1, 2)
    # make disjunctive cut
    alpha_1 = [0, 1]
    beta_1 = .5

    # make perturbed instance
    A_2 = np.array(
        [[1, -1],
         [-1, -1],
         [1, 0],
         [0, 1]]
    )
    b_2 = np.array([0, -2.75, .75, 0])
    # make children terms
    A_21 = np.vstack([A_2, [-1, 0]])
    b_21 = np.append(b_2, -1)
    A_22 = np.vstack([A_2, [1, 0]])
    b_22 = np.append(b_2, 2)
    # make disjunctive cut
    alpha_2 = [0, 1]
    beta_2_tight = 1

    # instantiate polyhedra
    op_1 = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
    lc_1 = Polyhedron2D(A=-A_11, b=-b_11)
    rc_1 = Polyhedron2D(A=-A_12, b=-b_12)
    op_2 = Polyhedron2D(A=-A_2, b=-b_2)
    lc_2 = Polyhedron2D(A=-A_21, b=-b_21)
    rc_2 = Polyhedron2D(A=-A_22, b=-b_22)
    f = Figure()
    f.add_polyhedron(op_1, color='red', show_int_points=False, linestyle='dotted')
    f.add_polyhedron(op_2, color='red', show_int_points=False, linestyle='solid')
    f.set_xlim([.25, 3.25])
    f.set_ylim([-.25, 1.75])

    # add disjunction and disjunctive cut
    f.add_line(alpha_1, beta_1, op_1.xlim + [0, 0.8], op_1.ylim + [0.2, 1.8],
               linestyle=':', color='green')
    f.add_line(alpha_2, beta_2_tight, op_1.xlim + [0, 0.8], op_1.ylim + [0.2, 1.8],
               linestyle='--', color='green')

    # shade in feasible regions
    f.ax.fill(lc_1.hrep.generators[:, 0], lc_1.hrep.generators[:, 1],
              color='red', alpha=0.3)
    f.ax.fill(lc_2.hrep.generators[:, 0], lc_2.hrep.generators[:, 1],
              color='red', alpha=0.2)
    f.ax.fill(rc_1.hrep.generators[:, 0], rc_1.hrep.generators[:, 1],
              color='red', alpha=0.3)
    f.ax.fill(rc_2.hrep.generators[:, 0], rc_2.hrep.generators[:, 1],
              color='red', alpha=0.2)

    # add term solution
    f.ax.plot(1, 1, 'ko', markersize=4)
    f.ax.text(1.065, .935, r"$\bar{x}^t$", ha='center', va='center', fontsize=12, color='black')

    # annotate
    # 1) perturb
    f.ax.annotate('', xy=(.75, -0.05), xytext=(1.25, -0.05),
                  arrowprops=dict(arrowstyle='-|>', color='black', lw=1.5), annotation_clip=False)
    f.ax.annotate('', xy=(2.75, -0.05), xytext=(2.5, -0.05),
                  arrowprops=dict(arrowstyle='-|>', color='black', lw=1.5), annotation_clip=False)
    f.ax.text(1.75, -.15, "1) perturb", ha='center', va='center', fontsize=12, color='black')
    f.ax.annotate('', xy=(1.25, -.075), xytext=(1.525, -.14),
                  arrowprops=dict(arrowstyle='-', color='black', lw=1))
    f.ax.annotate('', xy=(2.5, -.075), xytext=(1.975, -.14),
                  arrowprops=dict(arrowstyle='-', color='black', lw=1))
    f.ax.text(.28, -.2, "term becomes feasible", ha='left', va='center', fontsize=12, color='black')
    f.ax.annotate('', xy=(.85, .15), xytext=(.55, -.15),
                  arrowprops=dict(arrowstyle='-', color='black', lw=1, connectionstyle="arc3,rad=-.2"))


    # 2) Find Farkas certificate
    f.ax.annotate('', xy=(.99, 1.01), xytext=(.8, 1.4),
                  arrowprops=dict(arrowstyle='-', color='black', lw=1))
    f.ax.text(.515, 1.5, r"2) find $(\bar{x}^t, \bar{v}^t)$"+"\nprimal-dual optimal", ha='left', va='center', fontsize=12, color='black')

    # 3) parameterize
    f.ax.annotate('', xy=(2.11, 1), xytext=(2.11, .5),
                  arrowprops=dict(arrowstyle='-|>', color='black', lw=1))
    f.ax.text(2.13, .8, "3) parameterize", ha='left', va='center', fontsize=12, color='black')

    f.show(wait_for_click=False, filename='P17-18.png')

if False:

    # # P19-20
    # # make a perturbed polyhedron where a feasible basis becomes infeasible
    A_1 = np.array(
        [[1, -1],
         [-1, -1],
         [-1, -3],
         [1, 0],
         [0, 1]]
    )
    b_1 = np.array([0, -3, -5.5, 0, 0])
    # make children terms
    A_11 = np.vstack([A_1, [-1, 0]])
    b_11 = np.append(b_1, -1)
    A_12 = np.vstack([A_1, [1, 0]])
    b_12 = np.append(b_1, 2)
    # make disjunctive cut
    alpha_1 = [0, 1]
    beta_1 = 1

    # make perturbed instance
    A_2 = np.array(
        [[1, -1],
         [-1, -1],
         [-1, -3],
         [1, 0],
         [0, 1]]
    )
    b_2 = np.array([0, -3.5, -5.5, 0, 0])
    # make children terms
    A_21 = np.vstack([A_2, [-1, 0]])
    b_21 = np.append(b_2, -1)
    A_22 = np.vstack([A_2, [1, 0]])
    b_22 = np.append(b_2, 2)
    # make disjunctive cut
    alpha_2 = [0, 1]
    beta_2 = 1.5
    beta_2_tight = 7/6

    # instantiate polyhedra
    op_1 = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
    lc_1 = Polyhedron2D(A=-A_11, b=-b_11)
    rc_1 = Polyhedron2D(A=-A_12, b=-b_12)
    op_2 = Polyhedron2D(A=-A_2, b=-b_2)
    lc_2 = Polyhedron2D(A=-A_21, b=-b_21)
    rc_2 = Polyhedron2D(A=-A_22, b=-b_22)
    f = Figure()
    f.add_polyhedron(op_1, color='red', show_int_points=False, linestyle='dotted')
    f.add_polyhedron(op_2, color='red', show_int_points=False, linestyle='solid')
    f.set_xlim([-.25, 3.75])
    f.set_ylim([-.25, 1.6])

    # add disjunction and disjunctive cut
    f.add_line(alpha_1, beta_1, op_1.xlim + [0, 0.8], op_1.ylim + [0.2, 1.8],
               linestyle=':', color='green')
    f.add_line(alpha_2, beta_2, op_1.xlim + [0, 0.8], op_1.ylim + [0.2, 1.8],
               linestyle='-.', color='green')
    f.add_line(alpha_2, beta_2_tight, op_1.xlim + [0, 0.8], op_1.ylim + [0.2, 1.8],
               linestyle='--', color='green')

    # shade in feasible regions
    f.ax.fill(lc_1.hrep.generators[:, 0], lc_1.hrep.generators[:, 1],
              color='red', alpha=0.3)
    f.ax.fill(lc_2.hrep.generators[:, 0], lc_2.hrep.generators[:, 1],
              color='red', alpha=0.2)
    f.ax.fill(rc_1.hrep.generators[:, 0], rc_1.hrep.generators[:, 1],
              color='red', alpha=0.3)
    f.ax.fill(rc_2.hrep.generators[:, 0], rc_2.hrep.generators[:, 1],
              color='red', alpha=0.2)

    # add perturbed basis
    f.ax.annotate('', xy=(2, .8), xytext=(2, 1),
                  arrowprops=dict(arrowstyle='-|>', linestyle=':', color='blue', lw=1))
    f.ax.annotate('', xy=(2.15, .85), xytext=(2, 1),
                  arrowprops=dict(arrowstyle='-|>', linestyle=':', color='blue', lw=1))
    f.ax.annotate('', xy=(2, 1.3), xytext=(2, 1.5),
                  arrowprops=dict(arrowstyle='-|>', color='blue', lw=1))
    f.ax.annotate('', xy=(2.15, 1.35), xytext=(2, 1.5),
                  arrowprops=dict(arrowstyle='-|>', color='blue', lw=1))

    # annotate
    # 1) perturb
    f.ax.annotate('', xy=(2, 1.5), xytext=(2, 1),
                  arrowprops=dict(arrowstyle='-|>', color='black', lw=1,
                                  connectionstyle="arc3,rad=-.2"))
    f.ax.annotate('', xy=(2.61, .9), xytext=(2.25, .75),
                  arrowprops=dict(arrowstyle='-|>', color='black', lw=1))
    f.ax.text(2.38, 1.05, "1) perturb", ha='left', va='bottom', fontsize=12, color='black')
    f.ax.annotate('', xy=(2.4, .835), xytext=(2.42, 1.06),
                  arrowprops=dict(arrowstyle='-', color='black', lw=1))
    f.ax.annotate('', xy=(1.95, 1.25), xytext=(2.37, 1.09),
                  arrowprops=dict(arrowstyle='-', color='black', lw=1))
    f.ax.text(2.2, 1.425, "basis becomes infeasible", ha='left', va='center', fontsize=12, color='black')
    # 2) parameterize
    f.ax.annotate('', xy=(0, 1.5), xytext=(0, 1),
                  arrowprops=dict(arrowstyle='-|>', color='black', lw=1))
    f.ax.text(-.09, .95, "2) parameterize", ha='left', va='center', fontsize=12, color='black')

    # 3) tighten
    f.ax.annotate('', xy=(.75, 1.165), xytext=(0.75, 1.5),
                  arrowprops=dict(arrowstyle='-|>', color='black', lw=1))
    f.ax.text(.77, 1.4, "3) tighten", ha='left', va='center', fontsize=12, color='black')

    f.show(wait_for_click=False, filename='P19-20.png')

print()