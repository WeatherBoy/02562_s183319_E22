# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
# Introduction to rendering by professor
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
# Created by Felix Bo Caspersen, s183319 on Wed Aug 31 2022

#%%
import numpy as np
import matplotlib.pyplot as plt
import math
import random

width = 512
height = 384
im = np.random.random(size = (height, width, 3))
im = im*im # this is an approximate gamma correction because imshow expects non-linear intensities
plt.imshow(im)
plt.show()

#%%
def vec3(x,y,z):
    return np.array((x,y,z))

def unit_vector(v):
    len = math.sqrt(np.dot(v,v))
    return v / len

def background_color(v):
    u = 0.5*(1.0 + v[1])
    return u*vec3(0.7, 0.8, 0.9) + (1.0-u)*vec3(0.05, 0.05, 0.2)

# %%
ray_origin = vec3(0, 0, 0)
aspect = height/width
window_depth = 2.0
for row in range(height):
    for column in range(width):
        u = (column + 0.5) / width
        v = (row + 0.5) / height
        ray_direction = unit_vector(vec3(2.0*u-1.0, aspect*(2.0*v-1.0), -window_depth))
        im[height-row-1,column,:] = background_color( ray_direction )

# %%
def hit_sphere(ray_origin, ray_direction, center, radius):
    oc = ray_origin - center
    qb = np.dot(oc,ray_direction)
    qc = np.dot(oc,oc)-radius*radius
    discriminant = qb*qb-qc
    
    if (discriminant > 0):
        t = (-qb - math.sqrt(discriminant))
        
    if t > 0.00001:
        return t
    
    t = (-qb + math.sqrt(discriminant))
    if t > 0.0001:
        return t
    
    return 1.0e9

t = hit_sphere(ray_origin, ray_direction, center, radius)
if (t < 1.0e8):
    im[height-row-1,column,:] = vec3(1,0,0)
else:
    im[height-row-1,column,:] = background_color( ray_direction )

# %%
def pseudocolor(v):
    return 0.5*(vec3(1, 1, 1) + v)

hit_point = ray_origin + t * ray_direction
surface_normal = (1/radius) * (hit_point - center)
im[height-row-1,column,:] = pseudocolor(surface_normal)

#%%
def hit_array(ray_origin, ray_direction, centers, radii):
    t_min = 9.0e8
    t_min = -1
    hit_something = False
    for i in range(len(centers)):
        t = hit_sphere(ray_origin, ray_direction, centers[i], radii[i])
        if (t >= 0.0001 and t < t_min):
            t_min = t
            i_min = i
            hit_something = True
    return (hit_something, t_min, i_min)
