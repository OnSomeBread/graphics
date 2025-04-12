Display "Objects"  "Screen"  "rgbdouble"

Background 0.6 0.7 0.8

CameraUp 0 0 1
CameraAt 0 0 1
CameraEye -6 -10 6
CameraFOV  100

ObjectBegin "Axes"
XformPush
# X Axis
Color 1 0 0
Line 0 0 0  5 0 0

# Y Axis
Color 0 1 0
Line 0 0 0  0 5 0

# Z Axis
Color 0 0 1
Line 0 0 0  0 0 5
XformPop
ObjectEnd  # Axes

ObjectBegin "Arrow"
XformPush

Color 0.3 0.4 0.7
Cylinder .13 0 3 360

Rotate "Y" 45
Cylinder .08 0 1.2 360

Rotate "Y" -90
Cylinder .08 0 1.2 360

XformPop
ObjectEnd # "Arrow"

WorldBegin
AmbientLight 0.6 0.7 0.8 0.3
FarLight 0 1 -1  1 1 1 0.7

ObjectInstance "Axes"

ObjectInstance "Arrow"

# Branch 1
XformPush
Translate 0 0 3
Rotate "Y" 20
Rotate "X" 20
ObjectInstance "Arrow"

Translate 0 0 3
Rotate "Y" 45
Rotate "X" 45
ObjectInstance "Arrow"

Translate 0 0 3
Rotate "Y" 35
Rotate "X" 35
ObjectInstance "Arrow"
XformPop

# Branch 2
XformPush
Translate 0 0 3
ObjectInstance "Arrow"
XformPop

# Branch 3
XformPush
Translate 0 0 3
Rotate "X" -45
Rotate "Y" -45
ObjectInstance "Arrow"
XformPop

# Branch 4
Translate 0 0 3
Rotate "Y" 55
Rotate "X" 55
ObjectInstance "Arrow"

Translate 0 0 3
Rotate "Y" 50
Rotate "X" 50
ObjectInstance "Arrow"

Translate 0 0 3
Rotate "Y" 45
Rotate "X" 45
ObjectInstance "Arrow"

Translate 0 0 3
Rotate "Y" 35
Rotate "X" 35
ObjectInstance "Arrow"

Translate 0 0 3
Rotate "Y" 35
Rotate "X" 35
ObjectInstance "Arrow"

Translate 0 0 3
Rotate "Y" 35
Rotate "X" 35
ObjectInstance "Arrow"

Translate 0 0 3
Rotate "Y" 35
Rotate "X" 35
ObjectInstance "Arrow"

Translate 0 0 3
Rotate "Y" 25
Rotate "X" 25
ObjectInstance "Arrow"

Translate 0 0 3
Rotate "Y" 25
Rotate "X" 25
ObjectInstance "Arrow"

Translate 0 0 3
Rotate "Y" 30
Rotate "X" 30
ObjectInstance "Arrow"

Translate 0 0 3
Rotate "Y" 30
Rotate "X" 30
ObjectInstance "Arrow"

WorldEnd