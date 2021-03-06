set terminal postscript eps enhanced color solid lw 2 "Helvetica" 13
set title 'Speed-up para a matriz de ordem 256'
set xlabel 'Quantidade de Processos/Threads'
set ylabel 'Fator'

set autoscale
set xtics (1,2,3,4)
set xrange [0:5]
set yrange[0:5]
#set yrange [-1e+07:3.0e+08]
set grid
set key top right
#set key below

set output "speedup256.eps"

plot "testesmpi/resumo_o256.txt" using 1:(2364176/$2) with linespoints title 'MPI - recursivo' lt 1, \
  "testes/resumo_rec_o256.txt" using 1:(1571102/$2) with linespoints title 'OpenMP - recursivo' lt 2, \
  "testes/resumo_it_o256.txt" using 1:(607305/$2) with linespoints title 'OpenMP - iterativo' lt 3
