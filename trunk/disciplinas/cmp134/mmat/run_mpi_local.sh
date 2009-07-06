RUN=4
while [ $RUN -le 4 ]
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
			mpirun -np $RUN ./mm $RANK >> testesmpi/tempo_p${RUN}_o${RANK}.txt
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
