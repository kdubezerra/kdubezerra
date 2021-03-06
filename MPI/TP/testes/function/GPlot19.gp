set terminal postscript enhanced color solid lw 2 "Helvetica" 13
set title 'Execution time with 19 processors'
set xlabel 'Rank of the matrices'
set ylabel 'Time ({/Symbol m}s)'
#set xtics auto
#set ytics auto
set autoscale
set xtics (2,32,64,128,256,512,1024)
set xrange [-20:1050]
set yrange [-1.5e+06:4e+07]
set grid
set key top left
set key below
#set key width 1 box 9
#load 'teorico.gp'
#set terminal postscript enhanced color solid lw 2 "Times-Roman" 20
#set terminal postscript eps enhanced color
set output "P19.ps"
#plot 'plot_19.txt' using 2:3
#unset colorbox
#set style line 1 lt 2 lw 1
#set style line 2 lt 1 lw 1
set label "T_{19}(n) {/Symbol \273} 8202.7 + 67.6n + 1.1432n^2 + 0.1155n^{2.8}"  at 520,3.75e+07

#plot "plot_19.txt" using 5 title 'Actual time' with linespoints linetype 20 pointtype 5 pointsize 2, "plot_19.txt" using 3 title 'Theoretical estimation' linetype 3 with lines, 'plot_19.txt' using 3:5:($5-$7):($5+$7) title 'Standard deviation' with yerrorbars

#plot "plot_19.txt" using 5:($5-3):($5+$3) with yerr title 'Actual time' with linespoints linetype 20 pointtype 5 pointsize 2, "plot_19.txt" using 3 title 'Theoretical estimation' linetype 3 with lines, 'plot_19.txt' using 3:5:($5-$7):($5+$7) title 'Standard deviation' with yerrorbars

#plot "plot_19.txt" using 1:5:($5-7):($5+$7) with yerrorbars title 'Standard deviation', "plot_19.txt" using 5 with lines title 'Mean time' linetype 7, "plot_19.txt" using 9 with lines title 'Theoretical time' linetype 4, "plot_19.txt" using 3 title 'Theoretical estimation' linetype 3 with lines

#plot "plot_19.txt" using 1:5:($5-7):($5+$7) with yerrorbars title 'Standard deviation' pointtype 2, "plot_19.txt" using 5 with lines notitle lt 1, "plot_19.txt" using 3 title 'Theoretical estimation' linetype 8 with lines

#plot "plot_19.txt" using 3 title 'Theoretical estimation (fitted)'  with lines lt 1, "plot_19.txt" using 1:3:($3 - abs($9 - $3)):($3 + abs($9 - $3)) title 'Fitting Residuals' with yerrorbars lt 3, "plot_19.txt" using 5 with lines  title 'Mean time' lt 2, "plot_19.txt" using 1:5:($5-7):($5+$7) with yerrorbars title 'Standard deviation' lt 7

plot "plot_19.txt" using 3 title 'Theoretical estimation (fitted)'  with lines lt 1, "plot_19.txt" using 1:3:3:9 title 'Fitting Residuals' with yerrorbars lt 3, "plot_19.txt" using 5 with lines  title 'Mean time' lt 2, "plot_19.txt" using 1:5:($5-7):($5+$7) with yerrorbars title 'Standard deviation' lt 7

#plot "plot_19.txt" using 1:3:($3 - abs($9 - $3)):($3 + abs($9 - $3)) with yerrorbars

#linetype 1
# .
#ls 1
#ls 2