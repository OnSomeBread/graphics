Display "Objects"  "Screen"  "rgbdouble"

Background 0.6 0.7 0.8

CameraUp 0 0 1
CameraAt 0 0 0
CameraEye -10 -8 6
CameraFOV  30

WorldBegin

PolySet "PC"
      8 6 # Color cube - x y z  r g b
       -1.0 -1.0 -1.0 0.0 0.0 0.0  # Black
        1.0 -1.0 -1.0 1.0 0.0 0.0  # Red
       -1.0  1.0 -1.0 0.0 1.0 0.0  # Green
        1.0  1.0 -1.0 1.0 1.0 0.0  # Yellow
       -1.0 -1.0  1.0 0.0 0.0 1.0  # Blue
        1.0 -1.0  1.0 1.0 0.0 1.0  # Magenta
       -1.0  1.0  1.0 0.0 1.0 1.0  # Cyan
        1.0  1.0  1.0 1.0 1.0 1.0  # White

      0 2 3 1 -1 # Red Green Yellow face
      0 1 5 4 -1 # Red Blue Magenta face
      1 3 7 5 -1 # Red Yellow Magenta face
      3 2 6 7 -1 # Yellow Green Cyan face
      2 0 4 6 -1 # Green Blue Cyan face
      4 5 7 6 -1 # Blue Magenta Cyan face

WorldEnd