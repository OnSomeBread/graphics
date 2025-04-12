# This is a test/example rd file
# Array of white superquadric spheres
# Increasing e parameter from left to right
# Increasing n parameter from top to bottom.

Display "Superquadric Sphere" "Screen" "rgbobject"
Format 640 480

CameraFOV 1.7
CameraEye 0 0 500

Background 0.5 0.5 0.8
Clipping 490 510

WorldBegin

# Lights
FarLight -1 -1 -1  1.0  1.0  1.0  0.6  # Upper right
FarLight  0 -1 -1  1.0  1.0  1.0  0.3

#Surface Color
Color 1.0 1.0 1.0
Surface "matte"
Ka 0.5
Kd 0.5

OptionBool "Interpolate" off
OptionReal "Divisions" 24

# Row 0 Top - n = 0.0

XformPush
Translate -8.0 5.0 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 0.0 0.0 -1 1 360.0
XformPop

XformPush
Translate -4.0 5.0 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 0.0 0.5 -1 1 360.0
XformPop

XformPush
Translate 0.0 5.0 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 0.0 1.0 -1 1 360.0
XformPop

XformPush
Translate 4.0 5.0 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 0.0 2.0 -1 1 360.0
XformPop

XformPush
Translate 8.0 5.0 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 0.0 3.0 -1 1 360.0
XformPop

# Row 1 - n = 0.5

XformPush
Translate -8.0 2.5 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 0.5 0.0 -1 1 360.0
XformPop

XformPush
Translate -4.0 2.5 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 0.5 0.5 -1 1 360.0
XformPop

XformPush
Translate 0.0 2.5 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 0.5 1.0 -1 1 360.0
XformPop

XformPush
Translate 4.0 2.5 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 0.5 2.0 -1 1 360.0
XformPop

XformPush
Translate 8.0 2.5 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 0.5 3.0 -1 1 360.0
XformPop

# Row 2 - n = 1.0

XformPush
Translate -8.0 0.0 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 1.0 0.0 -1 1 360.0
XformPop

XformPush
Translate -4.0 0.0 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 1.0 0.5 -1 1 360.0
XformPop

XformPush
Translate 0.0 0.0 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 1.0 1.0 -1 1 360.0
XformPop

XformPush
Translate 4.0 0.0 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 1.0 2.0 -1 1 360.0
XformPop

XformPush
Translate 8.0 0.0 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 1.0 3.0 -1 1 360.0
XformPop

# Row 3 - n = 2.0

XformPush
Translate -8.0 -2.5 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 2.0 0.0 -1 1 360.0
XformPop

XformPush
Translate -4.0 -2.5 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 2.0 0.5 -1 1 360.0
XformPop

XformPush
Translate 0.0 -2.5 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 2.0 1.0 -1 1 360.0
XformPop

XformPush
Translate 4.0 -2.5 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 2.0 2.0 -1 1 360.0
XformPop

XformPush
Translate 8.0 -2.5 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 2.0 3.0 -1 1 360.0
XformPop

# Row 4 - n = 3.0

XformPush
Translate -8.0 -5.0 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 3.0 0.0 -1 1 360.0
XformPop

XformPush
Translate -4.0 -5.0 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 3.0 0.5 -1 1 360.0
XformPop

XformPush
Translate 0.0 -5.0 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 3.0 1.0 -1 1 360.0
XformPop

XformPush
Translate 4.0 -5.0 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 3.0 2.0 -1 1 360.0
XformPop

XformPush
Translate 8.0 -5.0 0
Rotate "X" -60.0
Rotate "Z" -30.0
SqSphere 1 3.0 3.0 -1 1 360.0
XformPop

WorldEnd
