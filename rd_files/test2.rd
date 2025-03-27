Display "Objects"  "Screen"  "rgbdouble"

#Format 1080 640

Background 0.6 0.7 0.8

CameraUp 0 0 1
CameraAt 0 0 0
CameraEye 1 -10 10
CameraFOV 50

ObjectBegin "Axis"
Color 1 0 0
Line 0 0 0 5 0 0

Color 0 1 0
Line 0 0 0 0 5 0

Color 0 0 1
Line 0 0 0 0 0 5
ObjectEnd # Axis


WorldBegin
AmbientLight 0.6 0.7 0.8 0.5
FarLight 0 1 -1  1 1 1 0.7
#PointLight

ObjectInstance "Axis"


Patch "Bezier" "PC" 2 2 # biquadratic patch with color - x y z  r g b
-0.5 -0.5 -1.0 1.0 0.0 0.0  # Red
0.0 -0.5 -0.5 1.0 1.0 0.0  # Yellow
0.5 -0.5 -1.0 1.0 1.0 1.0  # White

-0.7  0.0  0.0 1.0 0.0 1.0  # Magenta
0.0  0.2 -1.0 0.5 0.5 0.5  # Gray
0.7  0.0  0.0 0.0 1.0 0.0  # Green

-0.5  0.5  0.0 1.0 1.0 1.0  # White
0.1  0.7  1.0 1.0 1.0 0.0  # Yellow
0.5  0.5 -1.0 0.0 1.0 1.0  # Cyan


WorldEnd
