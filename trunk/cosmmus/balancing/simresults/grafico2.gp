set terminal postscript eps enhanced color solid lw 2 "Helvetica" 16
#set xlabel 'Step 2 algorithm'
set ylabel 'Migrations of players'
set autoscale
set grid
set key top right
set output "grafico2.eps"

# set terminal png transparent nocrop enhanced font arial 8 size 420,320 
# set output 'histograms.6.png'
set boxwidth 0.5 #absolute
set style fill solid 1.00 border -1
set style histogram rowstacked
set style data histograms
#set xtic rotate by -45
#set xtics 1000 nomirror
#set ytics 100 nomirror
#set mxtics 2
#set mytics 2
#set ytics 10
set yrange [0:250000]
#set ylabel "Total time"
#set xlabel "Session number"

#plot 'data' using 3 t "Server", '' using 4 t "Client", '' using 5:xtic(1) t "Network"
plot 'all_resumed.data' using 2 t "Walking Migrations" lc rgb '#333333', '' using 3:xtic(1) t "Still Migrations"  lc rgb '#888888'

pause -1 "Hit any key to continue"