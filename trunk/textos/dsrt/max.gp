set terminal postscript eps enhanced color solid lw 2 "Helvetica" 13
#set title 'title'
set xlabel 'Number of avatars'
set ylabel 'Maximum bandwidth per client (kilobytes/s)'
#set xtics auto
#set ytics auto
set autoscale
set xtics (25,50,75,100,125,150,175,200)
#set xrange [-20:1050]
#set yrange [-1e+07:3.0e+08]
set grid
set key top left
#set key below
#set key width 1 box 9
#load 'teorico.gp'
#set terminal postscript enhanced color solid lw 2 "Times-Roman" 20
#set terminal postscript eps enhanced color
set output "max.eps"
#plot 'plot_1.txt' using 2:3
#unset colorbox
#set style line 1 lt 2 lw 1
#set style line 2 lt 1 lw 1
#set label "T_1(n) {/Symbol \273} -2224.8 + 178.6n - 3.3571n^2 + 0.9406n^{2.8}" at 516,2.75e+08

#plot "plot_1.txt" using 5 title 'Actual time' with linespoints linetype 20 pointtype 5 pointsize 2, "plot_1.txt" using 3 title 'Theoretical estimation' linetype 3 with lines, 'plot_1.txt' using 3:5:($5-$7):($5+$7) title 'Standard deviation' with yerrorbars

#plot "plot_1.txt" using 5:($5-3):($5+$3) with yerr title 'Actual time' with linespoints linetype 20 pointtype 5 pointsize 2, "plot_1.txt" using 3 title 'Theoretical estimation' linetype 3 with lines, 'plot_1.txt' using 3:5:($5-$7):($5+$7) title 'Standard deviation' with yerrorbars

#plot "plot_1.txt" using 1:5:($5-7):($5+$7) with yerrorbars title 'Standard deviation', "plot_1.txt" using 5 with lines title 'Mean time' linetype 7, "plot_1.txt" using 9 with lines title 'Theoretical time' linetype 4, "plot_1.txt" using 3 title 'Theoretical estimation' linetype 3 with lines

#plot "plot_1.txt" using 1:5:($5-7):($5+$7) with yerrorbars title 'Standard deviation' pointtype 2, "plot_1.txt" using 5 with lines notitle lt 1, "plot_1.txt" using 3 title 'Theoretical estimation' linetype 8 with lines

#plot "plot_1.txt" using 3 title 'Theoretical estimation (fitted)'  with lines lt 1, "plot_1.txt" using 1:3:($3 - abs($9 - $3)):($3 + abs($9 - $3)) title 'Fitting Residuals' with yerrorbars lt 3, "plot_1.txt" using 5 with lines  title 'Mean time' lt 2, "plot_1.txt" using 1:5:($5-7):($5+$7) with yerrorbars title 'Standard deviation' lt 7

plot "max.data" using 1:($2/(2)) title 'No interest management' with linespoints lt 1, "max.data" using 1:($3/(2)) title 'Circle' with linespoints lt 3, "max.data" using 1:($4/(2)) title 'Circle with atenuation' with linespoints lt 3, "max.data" using 1:($5/(2)) with linespoints title 'Field of View' lt 2, "max.data" using 1:($6/(2)) with linespoints title 'A^3' lt 2 

#plot "plot_1.txt" using 1:3:($3 - abs($9 - $3)):($3 + abs($9 - $3)) with yerrorbars

#linetype 1
# .
#ls 1
#ls 2