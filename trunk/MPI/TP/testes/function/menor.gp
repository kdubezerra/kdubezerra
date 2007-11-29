set title 'Matrices` rank X Time'
set xlabel 'matrice`s rank'
set ylabel 'time (usec)'
set xtics auto
set ytics auto
set autoscale
set xrange [0:1024]
set grid
set key bottom Right
set key width 3 box 3
load 'teorico.gp'
set terminal postscript eps enhanced color
set output "eps_menor.eps"
plot 'res_menor.dat' using 1:($2 / 1000) title 'All times' w points \
  ps 1 pt 3 lc 2,\
'dat_menor.dat' using 1:2 title 'Bucket sort time' w linespoints \
  lw 1 ps 2 pt 4 lc 1 lt 1,\
'dat_menor.dat' using 1:2:($2-$3):($2+$3) \
  title 'Standard deviation' with yerrorbars  lc 3 lw 1 lt 1
#pause -1
