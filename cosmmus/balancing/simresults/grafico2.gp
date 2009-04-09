set terminal postscript eps enhanced color solid lw 2 "Helvetica" 14

set ylabel 'Migrations of players'
set autoscale
set grid
set key top right
set output "grafico2.eps"

set boxwidth 0.5 #absolute
set style fill solid 1.00 border -1
set style histogram rowstacked
set style data histograms

set yrange [0:250000]

plot 'all_resumed.data' using 2 t "Walking Migrations" lc rgb '#333333', '' using 3:xtic(1) t "Still Migrations"  lc rgb '#888888'

pause -1 "Hit any key to continue"