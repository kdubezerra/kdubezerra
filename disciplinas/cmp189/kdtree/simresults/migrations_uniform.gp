set terminal postscript eps enhanced color solid lw 2 "Helvetica" 18
set encoding iso_8859_1
#set title 'Migra\347\365es de jogadores entre servidores (sem pontos de interesse)'
set ylabel 'Player migrations'
set autoscale
set grid
set key top right
set output "migrations_uniform.eps"

set boxwidth 0.5
set style fill solid 1.00 border -1
set style histogram rowstacked
set style data histograms
set key off

set yrange [0:250000]

plot 'all_resumed_uniform.data' using ($2+$3):xtic(1) t "Migrations" lc rgb '#AAAAAA'

#pause -1 "Hit any key to continue"
