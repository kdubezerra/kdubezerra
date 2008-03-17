set grid
set key 470,14200
set xlabel "Clients (total)"
set ylabel "Bandwidth per client (bytes/s)"
plot 'avg_unop.txt' title "Average unoptimized" with linespoints

replot 'max_unop.txt' title "Maximum unoptimized" with linespoints

replot 'avg_op.txt' title "Average optimized" with linespoints

replot 'max_op.txt' title "Maximum optimized" with linespoints

set term postscript eps monochrome
set output "graf.eps"

replot
