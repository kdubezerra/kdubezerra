set terminal postscript enhanced color solid lw 2 "Helvetica" 13
set title 'Execution time with 1 processor'
set xlabel 'Rank of the matrices'
set ylabel 'Time ({/Symbol m}s)'

set autoscale
set xtics (2,32,64,128,256,512,1024)
set xrange [-20:1050]
set yrange [-1e+07:3.0e+08]
set grid
set key top left
set key below

set output "P1.ps"

set label "T_1(n) {/Symbol \273} -2224.8 + 178.6n - 3.3571n^2 + 0.9406n^{2.8}" at 516,2.75e+08

plot "resumo_it.txt" using 3 title 'Theoretical estimation (fitted)'  with lines lt 1, "plot_1.txt" using 1:3:3:9 title 'Fitting Residuals' with yerrorbars lt 3, "plot_1.txt" using 5 with lines  title 'Mean time' lt 2, "plot_1.txt" using 1:5:($5-7):($5+$7) with yerrorbars title 'Standard deviation' lt 7
