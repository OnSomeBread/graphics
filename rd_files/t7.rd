Display "Objects"  "Screen"  "rgbdouble"

Background 0.6 0.7 0.8

CameraUp 0 0 1
CameraAt 0 0 3
CameraEye -10 -8 6
#CameraEye -50 0 3
CameraFOV  30

ObjectBegin "Axes"
# X Axis
Color 1 0 0
Line 0 0 0  5 0 0

# Y Axis
Color 0 1 0
Line 0 0 0  0 5 0

# Z Axis
Color 0 0 1
Line 0 0 0  0 0 5

ObjectEnd  # Axes

ObjectBegin "Link"
XformPush
Translate 0 0 0.5
Scale 0.4 0.4 1.0
Sphere 0.5  -0.5 0.5  360
XformPop
ObjectEnd # Link

FrameBegin 1
WorldBegin


AmbientLight 0.6 0.7 0.8  0.5
FarLight 0 1 -1  1 1 1 0.7
#PointLight 0 -10 10  1 1 1 100
#PointLight 10 0 10   1 1 0 100
Color 1 1 1

ObjectInstance "Axes"

Color 1 1 0

ObjectInstance "Link"
Translate 0 0 1.0


Rotate "Y" 45
ObjectInstance "Link"
Translate 0 0 1.0

Rotate "Y" 45
ObjectInstance "Link"
Translate 0 0 1.0

XformPush
Rotate "Y" -45
ObjectInstance "Link"
Translate 0 0 1.0

Rotate "Y" 45
ObjectInstance "Link"
Translate 0 0 1.0

Rotate "Y" 45
ObjectInstance "Link"
Translate 0 0 1.0
XformPop

XformPush
Rotate "Y" -90
ObjectInstance "Link"
Translate 0 0 1.0

Rotate "Y" 45
ObjectInstance "Link"
Translate 0 0 1.0

Rotate "Y" 45
ObjectInstance "Link"
Translate 0 0 1.0

XformPop

WorldEnd
FrameEnd