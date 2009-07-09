set terminal postscript eps enhanced color solid lw 2 "Helvetica" 13
set title 'Tempos para a matriz de ordem 256'
set xlabel 'Quantidade de Processos/Threads'
set ylabel 'Tempo ({/Symbol m}s)'

set autoscale
set xtics (1,2,3,4)
set xrange [0:5]
set yrange[-10:3.0e+06]
#set yrange [-1e+07:3.0e+08]
set grid
set key top right
#set key below

set output "o256.eps"

plot "testesmpi/resumo_o256.txt" using 1:2 with lines title 'MPI - recursivo' lt 1, "testesmpi/resumo_o256.txt" using 1:2:3 with yerrorbars notitle lt 1 , \
  "testes/resumo_rec_o256.txt" using 1:2 with lines title 'OpenMP - recursivo' lt 2, "testes/resumo_rec_o256.txt" using 1:2:3 with yerrorbars notitle lt 2 , \
  "testes/resumo_it_o256.txt" using 1:2 with lines title 'OpenMP - iterativo' lt 3, "testes/resumo_it_o256.txt" using 1:2:3 with yerrorbars notitle lt 3
#plot "resumo_o4.txt" using 1 with lines



#, "plot_1.txt" using 1:3:3:9 title 'Fitting Residuals' with yerrorbars lt 3, "plot_1.txt" using 5 with lines  title 'Mean time' lt 2, "plot_1.txt" using 1:5:($5-7):($5+$7) with yerrorbars title 'Standard deviation' lt 7
