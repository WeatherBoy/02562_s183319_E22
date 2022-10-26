n_x = 5
n_y = 5
l = -2
r = 2
b = -2
t = 2


def calc_u(l, r, i, n_x):
    return l + (r - l)*(i + 0.5)/(n_x)

def calc_v(b, t, j, n_y):
    return b + (t - b)*(j + 0.5)/(n_y)

for i in range(n_x):
    print(f"u_{i} = {calc_u(l, r, i, n_x)}")

print()

for j in range(n_y):
    print(f"v_{j} = {calc_v(b, t, j, n_y)}")
