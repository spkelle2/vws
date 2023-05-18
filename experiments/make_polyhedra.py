from coinor.grumpy.polyhedron2D import Polyhedron2D, Figure

# P1
points = [[0, .5], [0, 0], [.25, -.25], [3, -.25], [3, 0], [1.25, 1.25]]
cp = [[0, 0], [3, 0], [1, 1]]
rays = []
c = [2/5, 1]
obj_val = 1.5

op = Polyhedron2D(points = points, rays = rays)
np = Polyhedron2D(points=cp, rays=rays)
f = Figure()
f.add_polyhedron(op, color='red', show_int_points=True, label="$P^1$")
f.add_polyhedron(np, color='blue', show_int_points=True, label="conv($S^1$)")
f.set_xlim([-.5, 3.5])
f.set_ylim([-.5, 2])
f.add_line(c, obj_val, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='green', label="$(\pi^1, \pi^1_0)$")
f.show(wait_for_click=False, filename='P1.png')

# P2
points = [[0, .5], [0, 0], [.25, -.25], [3, -.25], [3, .5], [1.5, 1.5]]
cp = [[0, 0], [3, 0], [2, 1], [1, 1]]
rays = []
c = [0, 1]
obj_val = 7/6

op = Polyhedron2D(points=points, rays=rays)
np = Polyhedron2D(points=cp, rays=rays)
f = Figure()
f.add_polyhedron(op, color='red', show_int_points=True, label="$P^2$")
f.add_polyhedron(np, color='blue', show_int_points=True, label="conv($S^2$)")
f.set_xlim([-.5, 3.5])
f.set_ylim([-.5, 2])
f.add_line(c, obj_val, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='green', label="$(\pi^2, \pi_0^2)$")
f.show(wait_for_click=False, filename='P2.png')


# P3
points = [[0, .5], [0, 0], [.25, -.25], [3, -.25], [3, 0], [1.25, 1.25]]
left_points = [[0, .5], [0, 0], [.25, -.25], [1, -.25], [1, 1.1]]
right_points = [[2, -.25], [3, -.25], [3, 0], [2, .7]]
rays = []
c = [1, 0]
lb = 2
ub = 1

op = Polyhedron2D(points = points, rays = rays)
lc = Polyhedron2D(points=left_points, rays=rays)
rc = Polyhedron2D(points=right_points, rays=rays)
f = Figure()
f.add_polyhedron(op, color='red', show_int_points=True, label='$P^1$', linestyle='dashed')
f.add_polyhedron(lc, color='red', show_int_points=True, label='$Q^{1,1}$')
f.add_polyhedron(rc, color='red', show_int_points=True, label='$Q^{1,2}$')
f.set_xlim([-.5, 3.5])
f.set_ylim([-.5, 2])
f.add_line(c, ub, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='black', label="$x_1 \leq 1$ or $x_1 \geq 2$")
f.add_line(c, lb, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='black')
f.show(wait_for_click=False, filename='P3.png')

# P4
points = [[0, .5], [0, 0], [.25, -.25], [3, -.25], [3, .5], [1.5, 1.5]]
left_points = [[0, .5], [0, 0], [.25, -.25], [1, -.25], [1, 7/6]]
right_points = [[2, -.25], [3, -.25], [3, .5], [2, 7/6]]
rays = []
c = [1, 0]
lb = 2
ub = 1

op = Polyhedron2D(points=points, rays=rays)
lc = Polyhedron2D(points=left_points, rays=rays)
rc = Polyhedron2D(points=right_points, rays=rays)
f = Figure()
f.add_polyhedron(op, color='red', show_int_points=True, label='$P^2$', linestyle='dashed')
f.add_polyhedron(lc, color='red', show_int_points=True, label='$Q^{2,1}$')
f.add_polyhedron(rc, color='red', show_int_points=True, label='$Q^{2,2}$')
f.set_xlim([-.5, 3.5])
f.set_ylim([-.5, 2])
f.add_line(c, ub, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='black', label="$x_1 \leq 1$ or $x_1 \geq 2$")
f.add_line(c, lb, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='black')
f.show(wait_for_click=False, filename='P4.png')

# P5
points = [[0, .5], [0, 0], [.25, -.25], [3, -.25], [3, 0], [1.25, 1.25]]
cp = [[0, 0], [3, 0], [1, 1]]
rays = []
c = [2/5, 1]
obj_val = 1.5

op = Polyhedron2D(points = points, rays = rays)
np = Polyhedron2D(points=cp, rays=rays)
f = Figure()
f.add_polyhedron(op, color='red', show_int_points=True, label="$P^1$")
f.add_polyhedron(np, color='blue', show_int_points=True, label="conv($S^1$)")
f.set_xlim([-.5, 3.5])
f.set_ylim([-.5, 2])
f.add_line(c, obj_val, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='green', label="$(\pi^1, \pi^1_0)$")
f.show(wait_for_click=False, filename='P5.png')

# P6
points = [[0, .5], [0, 0], [.25, -.25], [3, -.25], [3, .5], [1.5, 1.5]]
cp = [[0, 0], [3, 0], [2, 1], [1, 1]]
rays = []
c = [2/5, 1]
obj_val = 1.5

op = Polyhedron2D(points=points, rays=rays)
np = Polyhedron2D(points=cp, rays=rays)
f = Figure()
f.add_polyhedron(op, color='red', show_int_points=True, label="$P^2$")
f.add_polyhedron(np, color='blue', show_int_points=True, label="conv($S^2$)")
f.set_xlim([-.5, 3.5])
f.set_ylim([-.5, 2])
f.add_line(c, obj_val, op.xlim + [0, 0.8], op.ylim + [0.2, 1.8],
           linestyle='dashed', color='green', label="$(\pi^1, \pi_0^1)$")
f.show(wait_for_click=False, filename='P6.png')

print()