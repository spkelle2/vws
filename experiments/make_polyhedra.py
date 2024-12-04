from coinor.grumpy.polyhedron2D import Polyhedron2D, Figure
from matplotlib import pyplot as plt
import numpy as np

plt.rcParams['font.size'] = 12

# update plt to use latex fonts
plt.rcParams['text.usetex'] = True

# P7
# make a first polyhedron with a disjunctive cut
# make polyhedron
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
alpha = [0, 1]
beta = 1

# instantiate polyhedra
op = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
lc = Polyhedron2D(A=-A_11, b=-b_11)
rc = Polyhedron2D(A=-A_12, b=-b_12)
f = Figure()
f.add_polyhedron(op, color='red', show_int_points=True, label='$P^1$', linestyle='dotted')
f.add_polyhedron(lc, color='red', show_int_points=True, label='$Q^{1,1}$')
f.add_polyhedron(rc, color='red', show_int_points=True, label='$Q^{1,2}$')
f.set_xlim([-.5, 4.5])
f.set_ylim([-.5, 2.5])

# add disjunction and disjunctive cut
f.add_line(alpha, beta, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='green', label=r"$x_2 \leq 1$")
# f.ax.annotate('', xy=(1.5, 0.25), xytext=(1.5, 1),
#               arrowprops=dict(arrowstyle='-|>', color='green', lw=1, linestyle='--'))
f.add_line([1, 0], 1, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dotted', color='black', label="$x_1 = 1$ or $x_1 = 2$")
f.add_line([1, 0], 2, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dotted', color='black')
f.show(wait_for_click=False, filename='P7.png')

# P8
# make a first polyhedron with a disjunctive cut
# make polyhedron
A_1 = np.array(
    [[1, -1],
     [-1, -1],
     [1, 0],
     [0, 1]]
)
b_1 = np.array([0, -3.5, 0, 0])
# make children terms
A_11 = np.vstack([A_1, [-1, 0]])
b_11 = np.append(b_1, -1)
A_12 = np.vstack([A_1, [1, 0]])
b_12 = np.append(b_1, 2)
# make disjunctive cut
alpha = [0, 1]
beta = 1.5

# instantiate polyhedra
op = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
lc = Polyhedron2D(A=-A_11, b=-b_11)
rc = Polyhedron2D(A=-A_12, b=-b_12)
f = Figure()
f.add_polyhedron(op, color='red', show_int_points=True, label='$P^2$', linestyle='dotted')
f.add_polyhedron(lc, color='red', show_int_points=True, label='$Q^{2,1}$')
f.add_polyhedron(rc, color='red', show_int_points=True, label='$Q^{2,2}$')
f.set_xlim([-.5, 4.5])
f.set_ylim([-.5, 2.5])

# add disjunction and disjunctive cut
f.add_line(alpha, beta, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='green', label=r"$x_2 \leq 1.5$")
# f.ax.annotate('', xy=(1.5, 0.75), xytext=(1.5, 1.5),
#               arrowprops=dict(arrowstyle='-|>', color='green', lw=1, linestyle='--'))
f.add_line([1, 0], 1, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dotted', color='black', label="$x_1 = 1$ or $x_1 = 2$")
f.add_line([1, 0], 2, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dotted', color='black')
f.show(wait_for_click=False, filename='P8.png')

# P9
# make a matrix perturbed polyhedron with a parametrized disjunctive cut
# make polyhedron and children terms
A_1 = np.array(
    [[1.1, -1],
     [-1, -0.9],
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
alpha = [1, -9]
beta = -10

# instantiate polyhedra
op = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
lc = Polyhedron2D(A=-A_11, b=-b_11)
rc = Polyhedron2D(A=-A_12, b=-b_12)
f = Figure()
f.add_polyhedron(op, color='red', show_int_points=True, label='$P^3$', linestyle='dashed')
f.add_polyhedron(lc, color='red', show_int_points=True, label='$Q^{3,1}$')
f.add_polyhedron(rc, color='red', show_int_points=True, label='$Q^{3,2}$')
f.set_xlim([-.5, 4.5])
f.set_ylim([-.5, 2.5])

# add disjunction and disjunctive cut
f.add_line(alpha, beta, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='green', label=r"$x_1 - 9x_2 \geq -10$")
# f.ax.annotate('', xy=(1.5, .6), xytext=(1.4, 1.25),
#               arrowprops=dict(arrowstyle='-|>', color='green', lw=1, linestyle='--'))
f.add_line([1, 0], 1, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='black', label="$x_1 = 1$ or $x_1 = 2$")
f.add_line([1, 0], 2, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='black')
f.show(wait_for_click=False, filename='P9.png')

# P9.1
# make a matrix perturbed polyhedron with a parametrized disjunctive cut
# make polyhedron and children terms  they're equal at A_1_00 = 2 and A_1_11 = -.5
A_1 = np.array(
    [[3, -1],
     [-1, -0.25],
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
v_1 = np.array([1, 0, 0, 0, 1])
v_2 = np.array([0, 1, 0, 0, 1])
alpha = np.maximum(v_1 @ A_11, v_2 @ A_12)
beta = np.min(np.dot(v_1, b_11), np.dot(v_2, b_12))

# instantiate polyhedra
op = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
lc = Polyhedron2D(A=-A_11, b=-b_11)
rc = Polyhedron2D(A=-A_12, b=-b_12)
f = Figure()
f.add_polyhedron(op, color='red', show_int_points=True, label='$P^3$', linestyle='dashed')
f.add_polyhedron(lc, color='red', show_int_points=True, label='$Q^{3,1}$')
f.add_polyhedron(rc, color='red', show_int_points=True, label='$Q^{3,2}$')
f.set_xlim([-.5, 4.5])
f.set_ylim([-.5, 6])

# add disjunction and disjunctive cut
f.add_line(alpha, beta, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='green',
           label=f"$[{round(alpha[0], 2)}, {round(alpha[1], 2)}]^T x \geq {round(beta, 2)}$")
# f.ax.annotate('', xy=(1.5, .6), xytext=(1.4, 1.25),
#               arrowprops=dict(arrowstyle='-|>', color='green', lw=1, linestyle='--'))
f.add_line([1, 0], 1, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='black', label="$x_1 = 1$ or $x_1 = 2$")
f.add_line([1, 0], 2, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='black')
f.show(wait_for_click=False, filename='P91.png')

# P10
# make tightened disjunctive cut
alpha = [1, -9]
beta = -8.9

# instantiate polyhedra
f = Figure()
f.add_polyhedron(op, color='red', show_int_points=True, label='$P^3$', linestyle='dashed')
f.add_polyhedron(lc, color='red', show_int_points=True, label='$Q^{3,1}$')
f.add_polyhedron(rc, color='red', show_int_points=True, label='$Q^{3,2}$')
f.set_xlim([-.5, 4.5])
f.set_ylim([-.5, 2.5])

# add disjunction and disjunctive cut
f.add_line(alpha, beta, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='green', label=r"$x_1 - 9x_2 \geq -8.9$")
# f.ax.annotate('', xy=(1.5, .5), xytext=(1.4, 1.15),
#               arrowprops=dict(arrowstyle='-|>', color='green', lw=1, linestyle='--'))
f.add_line([1, 0], 1, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='black', label="$x_1 = 1$ or $x_1 = 2$")
f.add_line([1, 0], 2, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='black')
f.show(wait_for_click=False, filename='P10.png')

# P11
# make polyhedra
A_1 = np.array(
    [[1, -1],
     [-1, -1],
     [1, 0],
     [0, 1]]
)
b_1 = np.array([0, -3, 0, 0])
C_11 = np.array([
    [1, -1],
    [-1, 0]
])
C0_11 = np.array([0, -1])
C_12 = np.array([
    [-1, -1],
    [1, 0]
])
C0_12 = np.array([-3, 2])
# make disjunctive cut
alpha = [0, 1]
beta = 1
Alpha = np.array([alpha])
Alpha0 = np.array([beta])

# instantiate polyhedra
op = Polyhedron2D(A=-A_1, b=-b_1)  # negative because we use Ax >= b but grumpy assumes Ax <= b
lc = Polyhedron2D(A=-C_11, b=-C0_11)
rc = Polyhedron2D(A=-C_12, b=-C0_12)
cut = Polyhedron2D(A=-Alpha, b=-Alpha0)
f = Figure()
f.add_polyhedron(op, color='red', show_int_points=True, label='$P^1$', linestyle='dotted')
# todo: change self.ax.arrow 3rd and 4th arguments in add_polyhedron to ray[0]/(ray[0]**2 + ray[1]**2)**.5, ray[1]/(ray[0]**2 + ray[1]**2)**.5
f.add_polyhedron(lc, color='red', show_int_points=True, label='$\mathcal{R}^{1, 1} \cup \mathcal{R}^{1, 2}$')
f.add_polyhedron(rc, color='red', show_int_points=True)
f.set_xlim([-.5, 4.5])
f.set_ylim([-.5, 2.5])

# add local optima
f.ax.plot(1, 1, 'ko', markersize=4, label=r'$\mathcal{E}^{1, 1} \cup \mathcal{E}^{1, 2}$')
f.ax.plot(2, 1, 'ko', markersize=4)

# add disjunction and disjunctive cut
f.add_line(alpha, beta, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='green', label=r"$x_2 = 1$")
f.add_line([1, 0], 1, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dotted', color='black', label="$x_1 = 1$ or $x_1 = 2$")
f.add_line([1, 0], 2, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dotted', color='black')

# objective direction
f.ax.annotate('', xy=(1.5, 1), xytext=(1.5, 1.5),
              arrowprops=dict(arrowstyle='-|>', color='blue', lw=1))
f.add_line(alpha, 10, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           color='blue', label="$w$")
# f.ax.annotate('', xy=(0.25, 1.25), xytext=(1, 1.25),
#               arrowprops=dict(arrowstyle='-|>', color='black', lw=1, linestyle=':'))
# f.ax.annotate('', xy=(2.75, 1.25), xytext=(2, 1.25),
#               arrowprops=dict(arrowstyle='-|>', color='black', lw=1, linestyle=':'))
f.show(wait_for_click=False, filename='P11.png')

print()