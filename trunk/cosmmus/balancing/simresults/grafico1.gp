set terminal postscript eps enhanced color solid lw 2 "Helvetica" 18
#set title 'title'
set xlabel 'Time(s)'
set ylabel 'Total server migration of players'
#set xtics auto
#set ytics auto
set autoscale
#set xtics (25,50,75,100,125,150,175,200)
set xrange [100:1210]
set yrange [10000:25000]
set grid
set key top right
set pointsize 1.5
#set key below
#set key width 1 box 9
#load 'teorico.gp'
#set terminal postscript enhanced color solid lw 2 "Times-Roman" 20
#set terminal postscript eps enhanced color
set output "grafico1.eps"
#plot 'plot_1.txt' using 2:3
#unset colorbox
#set style line 1 lt 2 lw 1
#set style line 2 lt 1 lw 1
#set label "T_1(n) {/Symbol \273} -2224.8 + 178.6n - 3.3571n^2 + 0.9406n^{2.8}" at 516,2.75e+08

#nice colors:
#plot "max.data" using 1:($2/(2)) title 'No interest management' with linespoints lt 1, "max.data" using 1:($3/(2)) title 'Circle' with linespoints lt 3, "max.data" using 1:($4/(2)) title 'Circle with attenuation' with linespoints lt 8, "max.data" using 1:($5/(2)) with linespoints title 'Field of View' lt 7, "max.data" using 1:($6/(2)) with linespoints title 'A^3' lt 4

plot "smallpkl.data" using ($1 / 1000):(($2)+($3)) title 'ProGReGA' with linespoints lt 7, "smallpkhkl.data" using ($1 / 1000):(($2)+($3)) title 'ProGReGA-KH' with linespoints lt 7, "smallpkfkl.data" using ($1 / 1000):(($2)+($3)) title 'ProGReGA-KF' with linespoints lt 7, "smallbfakl.data" using ($1 / 1000):(($2)+($3)) with linespoints title 'BFBCT' lt 7

#plot "plot_1.txt" using 1:3:($3 - abs($9 - $3)):($3 + abs($9 - $3)) with yerrorbars

#linetype 1
# .
#ls 1
#ls 2
