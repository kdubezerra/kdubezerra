set terminal postscript eps enhanced color solid lw 2 "Helvetica" 13
set title 'Tempos com o OpenMP - algoritmo iterativo'
set xlabel 'Quantidade de Threads'
set ylabel 'Tempo ({/Symbol m}s)'

set autoscale
set xtics (1,2,3,4)
set xrange [0:5]
set yrange[-10:5.0e+07]
#set yrange [-1e+07:3.0e+08]
set grid
set key top right
#set key below

set output "omp_iter.eps"

plot "resumo_it_o4.txt" using 1:2 with lines title 'Ordem da Matriz: 4' lt 1, "resumo_it_o4.txt" using 1:2:3 with yerrorbars notitle lt 1 , \
  "resumo_it_o16.txt" using 1:2 with lines title '16' lt 2, "resumo_it_o16.txt" using 1:2:3 with yerrorbars notitle lt 2 , \
  "resumo_it_o64.txt" using 1:2 with lines title '64' lt 3, "resumo_it_o64.txt" using 1:2:3 with yerrorbars notitle lt 3 , \
  "resumo_it_o256.txt" using 1:2 with lines title '256' lt 6, "resumo_it_o256.txt" using 1:2:3 with yerrorbars notitle lt 6 , \
  "resumo_it_o512.txt" using 1:2 with lines title '512' lt 7, "resumo_it_o512.txt" using 1:2:3 with yerrorbars notitle lt 7 , \
  "resumo_it_o768.txt" using 1:2 with lines title '768' lt 8, "resumo_it_o768.txt" using 1:2:3 with yerrorbars notitle lt 8 , \
  "resumo_it_o1024.txt" using 1:2 with lines title '1024' lt 9, "resumo_it_o1024.txt" using 1:2:3 with yerrorbars notitle lt 9

#plot "resumo_it_o4.txt" using 1 with lines



#, "plot_1.txt" using 1:3:3:9 title 'Fitting Residuals' with yerrorbars lt 3, "plot_1.txt" using 5 with lines  title 'Mean time' lt 2, "plot_1.txt" using 1:5:($5-7):($5+$7) with yerrorbars title 'Standard deviation' lt 7
