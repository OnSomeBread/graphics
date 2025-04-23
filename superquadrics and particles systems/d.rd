Display "Objects"  "Screen"  "rgbdouble"

#Format 1080 640

Background 0.6 0.7 0.8

CameraUp 0 0 1
CameraAt 0 0 0
CameraEye -4 -5 5
CameraFOV 50

ObjectBegin "Axis"
Color 1 0 0
Line 0 0 0 5 0 0

Color 0 1 0
Line 0 0 0 0 5 0

Color 0 0 1
Line 0 0 0 0 0 5
ObjectEnd # Axis

ObjectBegin "Sprinkle"
Scale 0.05 0.2 0.05
SqSphere 0.1 1.0 1.0 -1.0 1.0 360
ObjectEnd

ObjectBegin "Sprinkle Circle"
XformPush
Translate 1.7 0.3 .8
ObjectInstance "Sprinkle"
XformPop

XformPush
Rotate "Z" 45
Translate 1.8 0.3 .8
ObjectInstance "Sprinkle"
XformPop


XformPush
Rotate "Z" 90
Translate 1.7 0.35 .8
ObjectInstance "Sprinkle"
XformPop

XformPush
Rotate "Z" 135
Translate 1.7 0.3 .8
ObjectInstance "Sprinkle"
XformPop

XformPush
Rotate "Z" 180
Translate 1.8 0.3 .8
ObjectInstance "Sprinkle"
XformPop

XformPush
Rotate "Z" 225
Translate 1.9 0.3 .8
ObjectInstance "Sprinkle"
XformPop

XformPush
Rotate "Z" 270
Translate 1.9 0.3 .8
ObjectInstance "Sprinkle"
XformPop

XformPush
Rotate "Z" 315
Translate 1.9 0.3 .8
ObjectInstance "Sprinkle"
XformPop
ObjectEnd # "Sprinkle Circle"

WorldBegin
AmbientLight 0.6 0.7 0.8 0.5
FarLight 0 .7 -1  1 1 1 0.8

#Surface "painted plastic"
Color 0.9 0.7 0.5

SqTorus 2.0 0.8 1.0 1.0 0 360 360

Color 1.0 0.2 0.2
ObjectInstance "Sprinkle Circle"

Color .2 0.8 0.8
XformPush
Rotate "Z" 22
Translate -.35 0 0 
ObjectInstance "Sprinkle Circle"
XformPop

Color .25 0.8 0.25
XformPush
Rotate "Z" -12
Translate .1 0 -.05 
ObjectInstance "Sprinkle Circle"
XformPop

Color 1 1 1
XformPush
Rotate "Z" -22
Translate -.45 0 -.05 
ObjectInstance "Sprinkle Circle"
XformPop

Color 0.8 .55 .7
XformPush
Rotate "Z" -22
Translate .45 -.1 -.05 
ObjectInstance "Sprinkle Circle"
XformPop

WorldEnd