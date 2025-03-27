Display "Objects"  "Screen"  "rgbdouble"

Background 0.6 0.7 0.8

CameraUp 0 0 1
CameraAt 0 0 7
CameraEye -6 -10 7
CameraFOV  80

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

WorldBegin
AmbientLight 0.6 0.7 0.8 0.3
FarLight 0 1 -1  1 1 1 0.7

ObjectInstance "Axes"

PolySet "P" 56 29
    # starting from the bottom left corner of the pants - front side
    0 0 0  #0
    2 0 0  #1
    2 0 5  #2
    3 0 5  #3
    3 0 0  #4
    5 0 0  #5
    5 0 6  #6
    0 0 6  #7
    
    # same set of coords but depth 1 - backside
    0 1 0  #8
    2 1 0  #9
    2 1 5  #10
    3 1 5  #11
    3 1 0  #12
    5 1 0  #13
    5 1 6  #14
    0 1 6  #15

    # starting bottom left corner shirt frontside
    0 0 7    #16
    5 0 7    #17
    5 0 13   #18
    6 0 13   #19
    6 0 10   #20
    8 0 10   #21
    8 0 13   #22
    5 0 15   #23
    0 0 15   #24
    -3 0 13  #25
    -3 0 10  #26
    -1 0 10  #27
    -1 0 13  #28
    0 0 13   #29

    # shirt backside
    0 1 7    #30
    5 1 7    #31
    5 1 13   #32
    6 1 13   #33
    6 1 10   #34
    8 1 10   #35
    8 1 13   #36
    5 1 15   #37
    0 1 15   #38
    -3 1 13  #39
    -3 1 10  #40
    -1 1 10  #41
    -1 1 13  #42
    0 1 13   #43

    # smile
    1.5 -0.1 11  #44
    2 -0.1 10    #45
    3 -0.1 10    #46
    3.5 -0.1 11  #47

    # left eye
    1.8 -0.2 13  #48
    1.8 -0.2 12  #49
    2 -0.2 12    #50
    2 -0.2 13    #51

    # right eye
    3.2 -0.2 13  #52
    3.2 -0.2 12  #53
    3 -0.2 12    #54
    3 -0.2 13    #55

    44 45 46 47 -1 # smile
    48 49 50 51 -1 # left eye
    52 53 54 55 -1 # right eye

    # pants faces
    0 1 2 3 4 5 6 7 -1 # front
    15 14 13 12 11 10 9 8 -1 # back
    7 6 14 15 -1 # top face 

    1 0 8 9 -1 # left shoe hole
    5 4 12 13 -1 # right shoe hole

    2 1 9 10 -1 # left inner
    2 3 11 10 -1 # middle inner
    11 12 4 3 -1 # right inner

    15 8 0 7 -1 # left outer
    6 5 13 14 -1 # right outer

    # shirt faces
    16 17 18 19 20 21 22 23 24 25 26 27 28 29 -1 # front
    30 31 32 33 34 35 36 37 38 39 40 41 42 43 -1 # back
    24 23 37 38 -1 # top
    16 17 31 30 -1 # bottom

    18 17 31 32 -1 # right inner left
    19 18 32 33 -1 # right inner top
    33 34 20 19 -1 # right inner right

    43 30 16 29 -1 # left inner right
    29 28 42 43 -1 # left inner top
    28 27 41 42 -1 # left inner left

    21 20 34 35 -1 # right hole
    27 26 40 41 -1 # left hole

    22 21 35 36 -1 # right outer
    39 40 26 25 -1 # left outer
    23 22 36 37 -1 # right top
    38 39 25 24 -1 # left top

WorldEnd