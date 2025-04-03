assign 1 - skew, twist, extrude, and sweeps on a polyset<br>
usage ./proj1 inputFileName outputFileName technique techniqueArgs<br>
possible techniques and args are<br>
skew skewX skewY<br>
twist degrees<br>
extrude extrudeAmount<br>
sweep steps degrees<br><br>

assign 2 - Marching Cubes Algorithm<br>
usage ./proj2 input_file output_file surfacelvl<br>

to make rd_file all of the .dat files have rows, cols, planes = 20, 20, 20<br>
./cview rows cols planes output_file > output.rd<br>
tests below are from file mc1.dat which is of 1 spherical field where mc2.dat is 2 spherical fields and mc3.dat is 4 spherical fields <br>
No Linear Interpolation and no Normals<br>
<img src="visualizations/sphere_no_lerp_no_normals.gif" width="450" /><br><br>
Linear Interpolation and no Normals<br>
<img src="visualizations/sphere_lerp_no_normals.gif" width="450" /><br><br>
Linear Interpolation and appoximated Normals (gif compression makes it appear less smooth)<br>
<img src="visualizations/smooth_sphere.gif" width="450" /><br><br>

assign 3 - Bezier curves and surfaces<br>
usage ./rdv rdfile<br>
calculated with real surface normals at each point with ∂/∂u(bernstein polynomials) X ∂/∂v(bernstein polynomials)<br>
the teapot below is only with 10 subdivisions but calculated surface normals fake the smoothness much better than appoximation<br>
<img src="visualizations/teapot.png" width="450" /><br><br>
the teapot below is 100 subdivisions
<img src="visualizations/100subdiv_teapot.png" width="450" /><br><br>