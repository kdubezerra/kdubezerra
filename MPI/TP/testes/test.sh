export PATH=/home/gppd/cebbezerra/my_mpi/install_dir/bin/:$PATH

#OAR -l walltime=59:00

cat $OAR_NODEFILE > $HOME/maquinas.txt
cat $HOME/maquinas.txt
lamboot $HOME/maquinas.txt

			mpirun -np 26 $HOME/TP/sm 1024 >> tempo_26_1024.txt
lamhalt
