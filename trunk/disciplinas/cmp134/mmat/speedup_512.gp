set terminal postscript eps enhanced color solid lw 2 "Helvetica" 13
set title 'Speed-up para a matriz de ordem 512'
set xlabel 'Quantidade de Processos/Threads'
set ylabel 'Tempo ({/Symbol m}s)'

set autoscale
set xtics (1,2,3,4)
set xrange [0:5]
set yrange[0:5]
#set yrange [-1e+07:3.0e+08]
set grid
set key top right
#set key below

set output "speedup512.eps"

plot "testesmpi/resumo_o512.txt" using 1:(18891750/$2) with linespoints title 'MPI - recursivo' lt 1, \
  "testes/resumo_rec_o512.txt" using 1:(12604689/$2) with linespoints title 'OpenMP - recursivo' lt 2, \
  "testes/resumo_it_o512.txt" using 1:(5005882/$2) with linespoints title 'OpenMP - iterativo' lt 3
