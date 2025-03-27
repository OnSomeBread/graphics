Display "Objects"  "Screen"  "rgbdouble"

Background 0.6 0.7 0.8

#Format 200 100

CameraUp 0 0 1
CameraAt 0 0 3
CameraEye -50 40 30
#CameraEye -50 0 3
CameraFOV  40

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

ObjectBegin "Row5"
Line -100 0 0  100 0 0
Line -100 10 0  100 10 0
Line -100 20 0  100 20 0
Line -100 30 0  100 30 0
Line -100 40 0  100 40 0
ObjectEnd # Row5

ObjectBegin "Grid"
Color 0 0 0
XformPush
XformPush
Translate 0 -100 0
ObjectInstance "Row5"
Translate 0 50 0
ObjectInstance "Row5"
Translate 0 50 0
ObjectInstance "Row5"
Translate 0 50 0
ObjectInstance "Row5"
XformPop
Rotate "Z" -90
Translate 0 -100 0
ObjectInstance "Row5"
Translate 0 50 0
ObjectInstance "Row5"
Translate 0 50 0
ObjectInstance "Row5"
Translate 0 50 0
ObjectInstance "Row5"
XformPop
ObjectEnd # Grid

ObjectBegin "Snowman"
XformPush
Color 1 1 1
Translate 0 0 5
Sphere 5  -5  5  360
Translate 0 0 7
Sphere 3  -3  3  360
Translate 0 0 4
Sphere 2  -2  2  360

Translate 0 2 0
Rotate "X" -90

Color 1.0 0.5 0.0
Cone 1.5 0.5 360
XformPop
ObjectEnd # Snowman

WorldBegin
AmbientLight 0.6 0.7 0.8  0.5
FarLight 0 -1 -1  1 1 1 0.7
ObjectInstance "Grid"
ObjectInstance "Axes"
Color 1 1 1
ObjectInstance "Snowman"

WorldEnd
