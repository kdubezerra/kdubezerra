export PATH=/home/gppd/cebbezerra/my_mpi/install_dir/bin/:$PATH

#OAR -l walltime=59:00

cat $OAR_NODEFILE > $HOME/maquinas.txt
cat $HOME/maquinas.txt
lamboot $HOME/maquinas.txt

RUN=1
while [ $RUN -le 26 ]
do
	echo ===============================
	echo ===============================
	echo EXECUTANDO COM $RUN PROCESSOS!!!
	RANK=2
	while [ $RANK -le 1024 ]
	do
		echo EXECUTANDO COM $RUN PROCESSOS E MATRIZ DE ORDEM $RANK...
		ITER=1
		while [ $ITER -le 10 ]  
		do
			echo EXECUTANDO COM $RUN PROCESSOS, MATRIZ DE ORDEM $RANK, ITERAÇÃO $ITER
			mpirun -np $RUN $HOME/TP/sm $RANK >> tempo_${RUN}_${RANK}.txt
			ITER=$((ITER+1))
		done
		RANK=$((RANK*2))
	done
	echo FIM DA EXECUCAO
	echo ===============================
	echo ===============================
	RUN=$((RUN+6))
done
lamhalt
