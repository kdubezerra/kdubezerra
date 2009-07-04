export PATH=/home/gppd/cebbezerra/my_mpi/install_dir/bin/:$PATH

#OAR -l walltime=59:00

cat $OAR_NODEFILE > $HOME/maquinas.txt
cat $HOME/maquinas.txt
lamboot $HOME/maquinas.txt

RUN=1
while [ $RUN -le 10 ]
do
	echo ===============================
	echo ===============================
	echo EXECUTANDO COM $RUN PROCESSOS!!!
	RANK=4
	while [ $RANK -le 1024 ]
	do
		echo EXECUTANDO COM $RUN PROCESSOS E MATRIZ DE ORDEM $RANK...
		ITER=1
		while [ $ITER -le 10 ]  
		do
			echo EXECUTANDO COM $RUN PROCESSOS, MATRIZ DE ORDEM $RANK, ITERACAO $ITER
			mpirun -np $RUN $HOME/TP/sm $RANK >> tempo_${RUN}_${RANK}.txt
			ITER=$((ITER+1))
		done
		if [ $RANK -lt 256 ]; then			
			RANK=$((RANK*4))
		else
			RANK=$((RANK+256))
		fi
	done
	echo FIM DA EXECUCAO
	echo ===============================
	echo ===============================
	RUN=$((RUN+1))
done
lamhalt
