set terminal postscript eps enhanced color solid lw 2 "Helvetica" 14

set ylabel 'Migrações de jogadores entre servidores (sem hotspots)'
set autoscale
set grid
set key top right
set output "migrations_uniform.eps"

set boxwidth 0.5
set style fill solid 1.00 border -1
set style histogram rowstacked
set style data histograms

set yrange [0:250000]

plot 'all_resumed_uniform.data' using ($2+$3):xtics(1) t "Migracoes" lc rgb '#5c5c5c'

pause -1 "Hit any key to continue"
