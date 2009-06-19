set terminal postscript eps enhanced color solid lw 2 "Helvetica" 14
set encoding iso_8859_1
#set term svg # Create an SVG image
set output 'grafico1.eps'

set style histogram gap 1
set style data histograms
set style fill solid 1.00 border -1

set boxwidth 1 # Probably 3-5.
set xlabel "Arquiteturas"
set ylabel "Milh\365es de Ciclos"
set autoscale
set grid


set key off # Unless you really want a key
# For this next line, lw is linewidth (2-4)?
#plot [XMIN:XMAX] 'myHistogramData' with boxes lw VALUE
#plot "ciclos" using 2 title 1 with boxes lw 2

plot "ciclos" using 2:xtic(1) lc rgb variable
#title "ciclos" with boxe

