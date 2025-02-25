Display "Objects"  "Screen"  "rgbdouble"

#Format 1080 640

Background 0.6 0.7 0.8

CameraUp 0 0 1
CameraAt 0 0 3
CameraEye -50 -40 30
CameraFOV 10

ObjectBegin "Axis"
Color 1 0 0
Line 0 0 0 5 0 0

Color 0 1 0
Line 0 0 0 0 5 0

Color 0 0 1
Line 0 0 0 0 0 5
ObjectEnd # Axis


ObjectBegin "Arm"
XformPush

Translate 0 0 0.5
Scale 0.4 0.4 1.0
Sphere 0.5 -0.5 0.5 360
XformPop
ObjectEnd #Arm

#FrameBegin 1
WorldBegin
AmbientLight 0.6 0.7 0.8 0.5
FarLight 0 1 -1  1 1 1 0.7
#PointLight

ObjectInstance "Axis"

Color 1 1 0
# start of the chain
ObjectInstance "Arm"

Translate 0 0 1
Rotate "Y" 45
ObjectInstance "Arm"

Translate 0 0 1
Rotate "Y" 45
ObjectInstance "Arm"

# begin branch 1
XformPush
Translate 0 0 1
Rotate "Y" -90
ObjectInstance "Arm"

Translate 0 0 1
Rotate "Y" 45
ObjectInstance "Arm"

Translate 0 0 1
Rotate "Y" 45
ObjectInstance "Arm"
XformPop

# begin branch 2
XformPush
Translate 0 0 1
Rotate "Y" -45
ObjectInstance "Arm"

Translate 0 0 1
Rotate "Y" 45
ObjectInstance "Arm"

Translate 0 0 1
Rotate "Y" 45
ObjectInstance "Arm"
XformPop


WorldEnd
#FrameEnd # 1
