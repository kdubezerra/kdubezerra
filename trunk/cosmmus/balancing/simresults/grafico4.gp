set terminal postscript eps enhanced color solid lw 1 "Helvetica" 14

set ylabel 'Average weight on each server'
set autoscale
set grid
set output "grafico4.eps"
set grid noxtics nomxtics ytics nomytics noztics nomztics \
 nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics

set boxwidth 0.7 absolute
set style fill solid 1.0 border -1
set style histogram rowstacked title offset 0, -6
set style data histograms
set key height 2 noautotitles #spacing 5 nobox outside below horizontal

set xlabel  offset character 0, 5, 0 #font "" textcolor lt -1 norotate
set xtic rotate #by 90 border 100

set yrange [0:300000]
set bmargin 10

plot newhistogram "Server 1", 'load_s1.data' using 2:xtic(1) t "Weight" lc rgb '#333333', '' using 3 t "Overhead" lc rgb '#888888', \
     newhistogram "Server 2", 'load_s2.data' using 2:xtic(1) notitle lc rgb '#333333', '' using 3 notitle lc rgb '#888888', \
     newhistogram "Server 3", 'load_s3.data' using 2:xtic(1) notitle lc rgb '#333333', '' using 3 notitle lc rgb '#888888', \
     newhistogram "Server 4", 'load_s4.data' using 2:xtic(1) notitle lc rgb '#333333', '' using 3 notitle lc rgb '#888888', \
     newhistogram "Server 5", 'load_s5.data' using 2:xtic(1) notitle lc rgb '#333333', '' using 3 notitle lc rgb '#888888', \
     newhistogram "Server 6", 'load_s6.data' using 2:xtic(1) notitle lc rgb '#333333', '' using 3 notitle lc rgb '#888888', \
     newhistogram "Server 7", 'load_s7.data' using 2:xtic(1) notitle lc rgb '#333333', '' using 3 notitle lc rgb '#888888', \
     newhistogram "Server 8", 'load_s8.data' using 2:xtic(1) notitle lc rgb '#333333', '' using 3 notitle lc rgb '#888888'

pause -1 "Hit any key to continue"