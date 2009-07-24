set terminal postscript eps enhanced color solid lw 2 "Helvetica" 18
set encoding iso_8859_1
#set title 'Desvio m\351dio da taxa de uso ideal dos servidores (com hotspots)'
set xlabel 'Time (s)'
set ylabel 'Deviation from the ideal balance'
#set xtics auto
#set ytics auto
set autoscale
#set xtics (25,50,75,100,125,150,175,200)
set xrange [80:1210]
set yrange [0.1:0.65]
set grid
set key top right
set pointsize 1.5
#set key below
#set key width 1 box 9
#load 'teorico.gp'
#set terminal postscript enhanced color solid lw 2 "Times-Roman" 20
#set terminal postscript eps enhanced color
set output "usagedeviation_hotspots.eps"
#plot 'plot_1.txt' using 2:3
#unset colorbox
#set style line 1 lt 2 lw 1
#set style line 2 lt 1 lw 1
#set label "T_1(n) {/Symbol \273} -2224.8 + 178.6n - 3.3571n^2 + 0.9406n^{2.8}" at 516,2.75e+08

#nice colors:
#plot "max.data" using 1:($2/(2)) title 'No interest management' with linespoints lt 1, "max.data" using 1:($3/(2)) title 'Circle' with linespoints lt 3, "max.data" using 1:($4/(2)) title 'Circle with attenuation' with linespoints lt 8, "max.data" using 1:($5/(2)) with linespoints title 'Field of View' lt 7, "max.data" using 1:($6/(2)) with linespoints title 'A^3' lt 4

#plot "progrega_kl.data" using ($1):($4) title 'ProGReGA' with linespoints lt 7, "progregakh_kl.data" using ($1):($4) title 'ProGReGA-KH' with linespoints lt 7, "progregakf_kl.data" using ($1):($4) title 'ProGReGA-KF' with linespoints lt 7, "bfa_kl.data" using ($1):($4) with linespoints title 'BFBCT' lt 7

#plot "progrega_kl.data" every 50 using ($1):($4) title 'ProGReGA' with lines lt 1, "progregakh_kl.data" every 50 using ($1):($4) title 'ProGReGA-KH' with lines lt 3, "progregakf_kl.data" every 50 using ($1):($4) title 'ProGReGA-KF' with lines lt 5, "bfa_kl.data" every 50 using ($1):($4) title 'BFBCT' with lines lt 7

plot "smallkdth.data" using (($1)/1000):($4) title 'KDTree' with linespoints lt 7, "smallbfh.data" using (($1)/1000):($4) title 'BFBCT' with linespoints lt 7, "smallahmedh.data" using (($1)/1000):($4) title 'Ahmed' with linespoints lt 7, "smallprh.data" using (($1)/1000):($4) title 'Progrega' with linespoints lt 7

#plot "plot_1.txt" using 1:3:($3 - abs($9 - $3)):($3 + abs($9 - $3)) with yerrorbars

#linetype 1
# .
#ls 1
#ls 2
