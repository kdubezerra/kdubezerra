set terminal postscript eps enhanced color solid lw 2 "Helvetica" 18
set encoding iso_8859_1
#set title 'title'
set xlabel 'N\372mero de avatares'
set ylabel 'Uso de banda m\351dio por cliente (bytes/s)'
#set xtics auto
#set ytics auto
set autoscale
set xtics (25,50,75,100,125,150,175,200)
#set xrange [-20:1050]
#set yrange [-1e+07:3.0e+08]
set grid
set key top left
set pointsize 1.5
#set key below
#set key width 1 box 9
#load 'teorico.gp'
#set terminal postscript enhanced color solid lw 2 "Times-Roman" 20
#set terminal postscript eps enhanced color
set output "avg.eps"
#plot 'plot_1.txt' using 2:3
#unset colorbox
#set style line 1 lt 2 lw 1
#set style line 2 lt 1 lw 1
#set label "T_1(n) {/Symbol \273} -2224.8 + 178.6n - 3.3571n^2 + 0.9406n^{2.8}" at 516,2.75e+08

#nice colors:
#plot "avg.data" using 1:($2/(2)) title 'No interest management' with linespoints lt 1, "avg.data" using 1:($3/(2)) title 'Circle' with linespoints lt 3, "avg.data" using 1:($4/(2)) title 'Circle with attenuation' with linespoints lt 8, "avg.data" using 1:($5/(2)) with linespoints title 'Field of View' lt 7, "avg.data" using 1:($6/(2)) with linespoints title 'A^3' lt 4

plot "avgd2.data" using 1:($2) title 'S/ gerenciamento de interesse' with linespoints lt 7, "avgd2.data" using 1:($3) title 'C\355rculo' with linespoints lt 7, "avgd2.data" using 1:($4) title 'C\355rculo com atenua\347\343o' with linespoints lt 7, "avgd2.data" using 1:($5) with linespoints title 'Campo de vis\343o' lt 7, "avgd2.data" using 1:($6) with linespoints title 'A^3' lt 7

#plot "plot_1.txt" using 1:3:($3 - abs($9 - $3)):($3 + abs($9 - $3)) with yerrorbars

#linetype 1
# .
#ls 1
#ls 2
