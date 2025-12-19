**Space engine**

Project's purpose is similating gravitational interaction in Universe

To start simulation, compile and run main.cpp file

File constants.h and constants.cpp contains global constants and thier values

For example:
SCALE - conut in meters for one conditional Raylib's unit
G - gravity constant

[Classes]

CameraController - class with Camera3D object inside with control

To speed up camera press LEFT_CTRL
To slow down camera press LEFT_ALT

Object - any object in Universe

To move and draw object, call tick(float dt, bool objectScale) method

[Physics]

F_val = G * m1 * m2 / r^2
direction = (obj1.pos - obj2.pos) / distance
F = direction * F_val

a = F / m
v += a

draw()
