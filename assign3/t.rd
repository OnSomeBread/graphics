Display "Objects"  "Screen"  "rgbdouble"

#Format 1080 640

Background 0.6 0.7 0.8

CameraUp 0 0 1
CameraAt 3 0 3
CameraEye 5 -30 5
CameraFOV 20

ObjectBegin "Axis"
Color 1 0 0
Line 0 0 0 5 0 0

Color 0 1 0
Line 0 0 0 0 5 0

Color 0 0 1
Line 0 0 0 0 0 5
ObjectEnd # Axis

ObjectBegin "Petal"
#Color 1 .714 .75686
Patch "Bezier" "P" 3 3
   0.0   0.0   0.0    # Base - origin (start here)
  -1.0  -2.5  -0.5    # Base - left side
   1.0  -2.5  -0.5    # Base - right side
   0.0  -2.5   0.0    # Base - center (same as origin)

  -3.0   0.0   1.5    # Mid-low - outward curve
  -1.0   0.5   2.5    # Mid-low - central rise
   1.0   0.5   2.5    # Mid-low - central rise
   3.0   0.0   1.5    # Mid-low - outward curve

  -2.5   2.5   2.0    # Mid-high - strong curve outward
  -1.0   3.0   3.5    # Mid-high - raised center
   1.0   3.0   3.5    # Mid-high - raised center
   2.5   2.5   2.0    # Mid-high - strong curve outward

  -1.0   4.5   0.5    # Tip - tapering inward
   0.0   5.5  -1.0    # Tip - central tip (slightly curved back)
   1.0   4.5   0.5    # Tip - tapering inward
   1.0   4.5   0.5    # Tip - tapering inward2
ObjectEnd # "Petal"



WorldBegin
    AmbientLight 0.6 0.7 0.8 0.5
    FarLight 0 1 -1  1 1 1 0.7
    #PointLight

    ObjectInstance "Axis"

    OptionReal "Divisions" 10
    OptionBool "Control" on

ObjectInstance "Petal"


WorldEnd
