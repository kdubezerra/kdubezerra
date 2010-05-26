set terminal postscript eps enhanced color solid lw 2 "Helvetica" 18
set encoding iso_8859_1
#set xlabel 'Instante (s)'
set xlabel 'Time (s)'
#set ylabel 'Total de migra\347\365es de jogadores entre servidoers'
set ylabel 'Total migrations of players between servers'
set autoscale
set xrange [100:1000]
set yrange [10000:25000]
set grid
set key top right
set pointsize 1.5
set output "grafico1.eps"

plot "smallpkl.data" using ($1 / 1000):(($2)+($3)) title 'ProGReGA' with linespoints lt 7, "smallpkhkl.data" using ($1 / 1000):(($2)+($3)) title 'ProGReGA-KH' with linespoints lt 7, "smallpkfkl.data" using ($1 / 1000):(($2)+($3)) title 'ProGReGA-KF' with linespoints lt 7, "smallbfakl.data" using ($1 / 1000):(($2)+($3)) with linespoints title 'BFBCT' lt 7, "smallahmed.data" using ($1 / 1000):(($2)+($3)) with linespoints title 'Ahmed' lt 7