set terminal postscript eps enhanced color solid lw 2 "Helvetica" 13
set title 'Speed-up para a matriz de ordem 1024'
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

set output "speedup1024.eps"

plot "testesmpi/resumo_o1024.txt" using 1:(151155668/$2) with linespoints title 'MPI - recursivo' lt 1, \
  "testes/resumo_rec_o1024.txt" using 1:(101453863/$2) with linespoints title 'OpenMP - recursivo' lt 2, \
  "testes/resumo_it_o1024.txt" using 1:(44728766/$2) with linespoints title 'OpenMP - iterativo' lt 3
