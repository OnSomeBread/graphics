assign 1 - skew, twist, extrude, and sweeps on a polyset<br>
usage ./proj1 inputFileName outputFileName technique techniqueArgs<br><br>

possible techniques and args are<br>
skew skewX skewY<br>
twist degrees<br>
extrude extrudeAmount<br>
sweep steps degrees<br><br>

assign 2 - Marching Cubes Algorithm<br>
usage ./proj2 input_file output_file surfacelvl<br>

to make rd_file all of the .dat files have rows, cols, planes = 20, 20, 20<br>
./cview rows cols planes output_file > output.rd<br>

No Linear Interpolation and no Normals<br>
<img src="sphere_no_lerp_no_normals.gif" width="400" /><br><br>
Linear Interpolation and no Normals<br>
<img src="sphere_lerp_no_normals.gif" width="400" /><br><br>
Linear Interpolation and Normals<br>
<img src="smooth_sphere.gif" width="400" /><br><br>
