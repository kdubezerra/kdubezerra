set terminal postscript eps enhanced color solid lw 1 "Helvetica" 14
set encoding iso_8859_1
#set xlabel 'Step 2 algorithm'
set ylabel 'Carga m\351dia em cada servidor'
set autoscale
set grid
set output "grafico4.eps"
set grid noxtics nomxtics ytics nomytics noztics nomztics \
 nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics
# set terminal png transparent nocrop enhanced font arial 8 size 420,320 
# set output 'histograms.6.png'
set boxwidth 0.7 absolute
set style fill solid 1.0 border -1
set style histogram rowstacked title offset 0, -6
set style data histograms
set key height 2 noautotitles #spacing 5 nobox outside below horizontal
#set xtic rotate by -45
#set xtics 1000 nomirror
#set ytics 100 nomirror
#set mxtics 2
#set mytics 2
#set ytics 10
#set yrange [0:250000]
#set ylabel "Total time"
#set xlabel "Session number"

#set xlabel  offset character 0, 32, 0 font "" textcolor lt -1 norotate
set xlabel  offset character 0, 5, 0 #font "" textcolor lt -1 norotate
set xtic rotate #by 90 border 100
#set xtics border in scale 1,0.5 nomirror rotate by -45  offset character 0, 0, 0 
set yrange [0:300000]
set bmargin 10
#set size 2.0, 2.0
#set nokey
#set xlabel  offset character 0, -2, 0 font "" textcolor lt -1 norotate

#plot 'data' using 3 t "Servidor", '' using 4 t "Client", '' using 5:xtic(1) t "Network"
#plot 'all_resumed.data' using 2 t "Walking Migrations", '' using 3:xtic(1) t "Still Migrations"
plot newhistogram "Servidor 1", 'load_s1.data' using 2:xtic(1) t "Carga" lc rgb '#333333', '' using 3 t "Overhead" lc rgb '#888888', \
     newhistogram "Servidor 2", 'load_s2.data' using 2:xtic(1) notitle lc rgb '#333333', '' using 3 notitle lc rgb '#888888', \
     newhistogram "Servidor 3", 'load_s3.data' using 2:xtic(1) notitle lc rgb '#333333', '' using 3 notitle lc rgb '#888888', \
     newhistogram "Servidor 4", 'load_s4.data' using 2:xtic(1) notitle lc rgb '#333333', '' using 3 notitle lc rgb '#888888', \
     newhistogram "Servidor 5", 'load_s5.data' using 2:xtic(1) notitle lc rgb '#333333', '' using 3 notitle lc rgb '#888888', \
     newhistogram "Servidor 6", 'load_s6.data' using 2:xtic(1) notitle lc rgb '#333333', '' using 3 notitle lc rgb '#888888', \
     newhistogram "Servidor 7", 'load_s7.data' using 2:xtic(1) notitle lc rgb '#333333', '' using 3 notitle lc rgb '#888888', \
     newhistogram "Servidor 8", 'load_s8.data' using 2:xtic(1) notitle lc rgb '#333333', '' using 3 notitle lc rgb '#888888'



# set terminal png transparent nocrop enhanced font arial 8 size 420,320 
# set output 'histograms.8.png'
#set border 3 front linetype -1 linewidth 1.000
#set boxwidth 0.8 absolute
#set style fill   solid 1.00 noborder
#set grid nopolar
#set grid noxtics nomxtics ytics nomytics noztics nomztics \
 #nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics
#set grid layerdefault   linetype 0 linewidth 1.000,  linetype 0 linewidth 1.000
#set key bmargin center horizontal Left reverse enhanced autotitles columnhead nobox
#set style histogram rowstacked title  offset character 2, 0.25, 0
#set datafile missing '-'
#set style data histograms
#set xtics border in scale 1,0.5 nomirror rotate by -45  offset character 0, 0, 0 
#set xtics   ("1891-1900" 0.00000, "1901-1910" 1.00000, "1911-1920" 2.00000, "1921-1930" 3.00000, "1931-1940" 4.00000, "1941-1950" 5.00000, "1951-1960" 6.00000, "1961-1970" 7.00000, "1891-1900" 9.00000, "1901-1910" 10.0000, "1911-1920" 11.0000, "1921-1930" 12.0000, "1931-1940" 13.0000, "1941-1950" 14.0000, "1951-1960" 15.0000, "1961-1970" 16.0000, "1891-1900" 18.0000, "1901-1910" 19.0000, "1911-1920" 20.0000, "1921-1930" 21.0000, "1931-1940" 22.0000, "1941-1950" 23.0000, "1951-1960" 24.0000, "1961-1970" 25.0000)
#set ytics border in scale 0,0 mirror norotate  offset character 0, 0, 0 autofreq 
#set ztics border in scale 0,0 nomirror norotate  offset character 0, 0, 0 autofreq 
#set cbtics border in scale 0,0 mirror norotate  offset character 0, 0, 0 autofreq 
#set title "Immigration from different regions\n(give each histogram a separate title)" 
#set xlabel "(Same plot using rowstacked rather than clustered histogram)" 
#set xlabel  offset character 0, -2, 0 font "" textcolor lt -1 norotate
#set ylabel "Immigration by decade" 
#set yrange [ 0.00000 : 900000. ] noreverse nowriteback
#i = 24
#plot newhistogram "Northern Europe", 'immigration.dat' using 6:xtic(1) t 6, '' u 13 t 13, '' u 14 t 14, newhistogram "Southern Europe", '' u 9:xtic(1) t 9, '' u 17 t 17, '' u 22 t 22, newhistogram "British Isles", '' u 10:xtic(1) t 10, '' u 21 t 21

