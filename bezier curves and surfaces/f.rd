Display "Objects"  "Screen"  "rgbdouble"

#Format 1080 640

Background 0.6 0.7 0.8

CameraUp 0 0 1
CameraAt 3 0 3
CameraEye -4 -15 25
CameraFOV 50

ObjectBegin "Axis"
Color 1 0 0
Line 0 0 0 5 0 0

Color 0 1 0
Line 0 0 0 0 5 0

Color 0 0 1
Line 0 0 0 0 0 5
ObjectEnd # Axis

ObjectBegin "Petal"
    Patch "Bezier" "P" 3 3
        -1.0   0.0   0.0
        -1.0  -2.5  -0.5
        1.0  -2.5  -0.5 
        2.0  -2.5   0.0

        -3.0   0.0   1.5 
        -1.0   0.5   2.5 
        1.0   0.5   2.5 
        3.0   0.0   1.5  

        -2.5   2.5   2.0 
        -1.0   3.0   3.5 
        1.0   3.0   3.5 
        2.5   2.5   2.0 

        -1.0  4.0   0.5 
        0.0   5.0  -1.0 
        1.0   4.0   0.5 
        1.0   4.0   0.5  
ObjectEnd # "Petal"

ObjectBegin "Petal2"
    Patch "Bezier" "P" 3 3
        -1.0 -2.5   0.0
        0.0 -2.5  -0.5
        1.0  -2.5  -0.5 
        2.0  -2.5   0.0

        -3.0   0.0   0.5
        -1.0   0.5   1.5
        1.0   0.5   1.5
        3.0   0.0   0.5 

        -1.5  0.5   2.0
        -1.0  2.5   3.5
        1.0   2.5   3.5
        1.5   0.5   2.0

        -2.0  2.0   1.5
        -1.0  4.0   1.5
        1.0   2.0   2.5
        2.0   4.0   1.5
ObjectEnd # "Petal2"

ObjectBegin "Petal3"
    Patch "Bezier" "P" 3 3
        -1.0 -1.5   0.0 # 1 .714 .75686
        0.0 -1.5  -0.5  # 1 .714 .75686
        1.0  -1.5  -0.5 # 1 .714 .75686 
        2.0  -1.5   0.0 # 1 .714 .75686

        -3.5   0.0   0.5 # 1 .714 .75686
        -1.0   0.5   1.5 # 1 .714 .75686
        1.0   0.5   1.5  # 1 .714 .75686
        3.5   0.0   0.5  # 1 .714 .75686

        -3.0  0.5   2.0 # 1 .714 .75686
        -1.0  2.5   3.5 # 1 .714 .75686
        1.0   0.5   3.5 # 1 .714 .75686
        3.0   2.5   2.0 # 1 .714 .75686

        -2.0  1.0   2.0 # 1 .8 .88
        0.0  3.0   3.5  # 1 .8 .88
        0.0   1.0   3.5 # 1 .8 .88
        2.0   3.0   2.0 # 1 .8 .88
ObjectEnd # "Petal3"

ObjectBegin "Petal4"
    Patch "Bezier" "P" 3 3
        -1.0 -1.5   0.0 # 1 .714 .75686
        0.0 -1.5  -0.5  # 1 .714 .75686
        1.0  -1.5  -0.5 # 1 .714 .75686
        2.0  -1.5   0.0 # 1 .714 .75686

        -2.5   0.0   0.5 # 1 .714 .75686
        -1.0   0.5   1.5 # 1 .714 .75686
        1.0   0.5   1.5  # 1 .714 .75686
        2.5   0.0   0.5  # 1 .714 .75686

        -3.0  0.5   2.0 # 1 .714 .75686
        -1.0  2.5   3.5 # 1 .714 .75686
        1.0   0.5   3.5 # 1 .714 .75686
        3.0   2.5   2.0 # 1 .714 .75686

        -2.0  0.0   2.0 # 1 .85 .9
        0.0  -0.5   3.5 # 1 .85 .9
        0.0  -0.5   3.5 # 1 .85 .9
        2.0   0.0   2.0 # 1 .85 .9
ObjectEnd # "Petal4"


WorldBegin
    AmbientLight 0.6 0.7 0.8 0.5
    FarLight 0 .7 -1  1 1 1 0.8

#    ObjectInstance "Axis"

#    OptionReal "Divisions" 100
#    OptionBool "Control" on
    OptionBool "DoubleSide" on

    Color 1 .714 .75686
    Sphere 1 -1 1 360

    # INNER LAYER
    XformPush
    Translate 0 2 0
    Scale .4 .4 .8
    ObjectInstance "Petal4"
    Translate 0 -1 0
    Scale .8 .8 .8
    ObjectInstance "Petal4"
    Translate 0 -1 0
    Scale .7 .7 .7
    ObjectInstance "Petal4"
    XformPop

    XformPush
    Rotate "Z" 45
    Translate 0 2 0
    Scale .4 .4 .8
    ObjectInstance "Petal4"
    Translate 0 -1 0
    Scale .8 .8 .8
    ObjectInstance "Petal4"
    Translate 0 -1 0
    Scale .7 .7 .7
    ObjectInstance "Petal4"
    XformPop

    XformPush
    Rotate "Z" 90
    Translate 0 2 0
    Scale .4 .4 .8
    ObjectInstance "Petal4"
    Translate 0 -1 0
    Scale .8 .8 .8
    ObjectInstance "Petal4"
    Translate 0 -1 0
    Scale .7 .7 .7
    ObjectInstance "Petal4"
    XformPop

    XformPush
    Rotate "Z" 135
    Translate 0 2 0
    Scale .4 .4 .8
    ObjectInstance "Petal4"
    Translate 0 -1 0
    Scale .8 .8 .8
    ObjectInstance "Petal4"
    Translate 0 -1 0
    Scale .7 .7 .7
    ObjectInstance "Petal4"
    XformPop

    XformPush
    Rotate "Z" 180
    Translate 0 2 0
    Scale .4 .4 .8
    ObjectInstance "Petal4"
    Translate 0 -1 0
    Scale .8 .8 .8
    ObjectInstance "Petal4"
    Translate 0 -1 0
    Scale .7 .7 .7
    ObjectInstance "Petal4"
    XformPop

    XformPush
    Rotate "Z" 225
    Translate 0 2 0
    Scale .4 .4 .8
    ObjectInstance "Petal4"
    Translate 0 -1 0
    Scale .8 .8 .8
    ObjectInstance "Petal4"
    Translate 0 -1 0
    Scale .7 .7 .7
    ObjectInstance "Petal4"
    XformPop

    XformPush
    Rotate "Z" 270
    Translate 0 2 0
    Scale .4 .4 .8
    ObjectInstance "Petal4"
    Translate 0 -1 0
    Scale .8 .8 .8
    ObjectInstance "Petal4"
    Translate 0 -1 0
    Scale .7 .7 .7
    ObjectInstance "Petal4"
    XformPop

    XformPush
    Rotate "Z" 315
    Translate 0 2 0
    Scale .4 .4 .8
    ObjectInstance "Petal4"
    Translate 0 -1 0
    Scale .8 .8 .8
    ObjectInstance "Petal4"
    Translate 0 -1 0
    Scale .7 .7 .7
    ObjectInstance "Petal4"
    XformPop
    # END INNER LAYER

    # START MIDDLE LAYER
    XformPush
    Translate 0 2.5 0
    Scale .8 .8 .8
    Rotate "X" 5
    ObjectInstance "Petal3"
    XformPop

    XformPush
    Rotate "Z" 72
    Translate 0 2.5 0
    Scale .8 .8 .8
    Rotate "X" 5
    ObjectInstance "Petal3"
    XformPop

    XformPush
    Rotate "Z" 144
    Translate 0 2.5 0
    Scale .8 .8 .8
    Rotate "X" 5
    ObjectInstance "Petal3"
    XformPop

    XformPush
    Rotate "Z" 216
    Translate 0 2.5 0
    Scale .8 .8 .8
    Rotate "X" 5
    ObjectInstance "Petal3"
    XformPop

    XformPush
    Rotate "Z" 288
    Translate 0 2.5 0
    Scale .8 .8 .8
    Rotate "X" 5
    ObjectInstance "Petal3"
    XformPop
    # END MIDDLE LAYER


    # OUTER LAYER
    XformPush
    Translate 0 3.9 0
    ObjectInstance "Petal"
    XformPop

    XformPush
    Rotate "Z" 60
    Translate 0 3.9 0
    ObjectInstance "Petal2"
    XformPop

    XformPush
    Rotate "Z" 120
    Translate 0 3.9 0
    ObjectInstance "Petal"
    XformPop

    XformPush
    Rotate "Z" 180
    Translate 0 3.9 0
    ObjectInstance "Petal2"
    XformPop

    XformPush
    Rotate "Z" 240
    Translate 0 3.9 0
    ObjectInstance "Petal"
    XformPop

    XformPush
    Rotate "Z" 300
    Translate 0 3.9 0
    ObjectInstance "Petal2"
    XformPop
    # END OUTER LAYER

#ObjectInstance "Petal2"

#    Color 1 0 0
#    Curve "Bezier" "P" 3
#        -1.0 -1.5   0.0
#        0.0 -1.5  -0.5
#        1.0  -1.5  -0.5 
#        2.0  -1.5   0.0
#
#    Color 0 1 0
#    Curve "Bezier" "P" 3
#        -3.5   0.0   0.5
#        -1.0   0.5   1.5
#        1.0   0.5   1.5
#        3.5   0.0   0.5 
#
#    Color 0 0 1
#    Curve "Bezier" "P" 3
#        -3.0  0.5   2.0
#        -1.0  2.5   3.5
#        1.0   0.5   3.5
#        3.0   2.5   2.0
#
#    Color 0 0 0
#    Curve "Bezier" "P" 3
#        -1.0  0.8   2.0
#        0.0  2.8   3.5
#        0.0   0.8   3.5
#        1.0   2.8   2.0

WorldEnd
