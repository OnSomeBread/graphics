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

ObjectBegin "Branch"
XformPush

Color 0.3 0.4 0.7
Cylinder .25 0 3 360

XformPop
ObjectEnd # "Branch"

WorldBegin
AmbientLight 0.6 0.7 0.8 0.3
FarLight 0 1 -1  1 1 1 0.7

ObjectInstance "Axes"

ObjectInstance "Branch"

Translate 0 0 3
Rotate "Y" 55
Rotate "X" 55
ObjectInstance "Branch"

Translate 0 0 3
Rotate "Y" 50
Rotate "X" 50
ObjectInstance "Branch"

Translate 0 0 3
Rotate "Y" 45
Rotate "X" 45
ObjectInstance "Branch"

Translate 0 0 3
Rotate "Y" 35
Rotate "X" 35
ObjectInstance "Branch"

Translate 0 0 3
Rotate "Y" 35
Rotate "X" 35
ObjectInstance "Branch"

Translate 0 0 3
Rotate "Y" 35
Rotate "X" 35
ObjectInstance "Branch"

Translate 0 0 3
Rotate "Y" 35
Rotate "X" 35
ObjectInstance "Branch"

Translate 0 0 3
Rotate "Y" 25
Rotate "X" 25
ObjectInstance "Branch"

Translate 0 0 3
Rotate "Y" 25
Rotate "X" 25
ObjectInstance "Branch"

Translate 0 0 3
Rotate "Y" 30
Rotate "X" 30
ObjectInstance "Branch"

Translate 0 0 3
Rotate "Y" 30
Rotate "X" 30
ObjectInstance "Branch"

WorldEnd