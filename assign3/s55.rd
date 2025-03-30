Display "Curvetest" "Screen" "rgbsingle"

WorldBegin

Color 0.5 0.5 0.5

OptionBool "Control" true
OptionReal "Divisions" 80

#Degree 2 curve
Color 1.0 1.0 1.0
Curve "Bezier" "P"
2
-300 200 -320
-250 280 -320
-200 200 -320


#Degree 3 curve
Color 1.0 1.0 1.0
Curve "Bezier" "P"
3
-150 150 -320 
-100 280 -320
-50 150 -320
 50 150 -320 

#Degree 5 curve
Color 1.0 1.0 1.0
Curve "Bezier" "P"
5
100 0 -320 
100 300 -320
300 300 -320
300 100 -320
200 100 -320
200 200 -320

OptionBool "Control" off

Curve "Bezier" "PC"
3
-0.5 -0.25 -1.0 1.0 0.0 0.0
-0.5  0.25 -1.0 1.0 1.0 0.0
 0.5 -0.75 -1.0 1.0 1.0 1.0
 0.5 -0.25 -1.0 0.0 1.0 0.0

WorldEnd

