Bezier curves and surfaces<br>
usage ./rdv rdfile<br>
calculated with real surface normals at each point with ∂/∂u(bernstein polynomials) X ∂/∂v(bernstein polynomials)<br>
the teapot below is only with 10 subdivisions but calculated surface normals fake the smoothness much better than appoximation<br>
<img src="visualizations/teapot.png" width="500" /><br><br>

Marching Cubes Algorithm<br>
usage ./proj2 input_file output_file surfacelvl<br>

to make rd_file all of the .dat files have rows, cols, planes = 20, 20, 20<br>
./cview rows cols planes output_file > output.rd<br>
tests below are from file mc1.dat which is of 1 spherical field where mc2.dat is 2 spherical fields and mc3.dat is 4 spherical fields <br>
No Linear Interpolation and no Normals<br>
<img src="visualizations/sphere_no_lerp_no_normals.gif" width="400" /><br><br>
Linear Interpolation and no Normals<br>
<img src="visualizations/sphere_lerp_no_normals.gif" width="400" /><br><br>
Linear Interpolation and appoximated Normals (gif compression makes it appear less smooth)<br>
<img src="visualizations/smooth_sphere.gif" width="400" /><br><br>
