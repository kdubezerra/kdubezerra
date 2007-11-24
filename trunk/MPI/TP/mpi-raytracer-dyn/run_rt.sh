export PATH=/usr/local/lam-7.1.2/bin:$PATH

#OAR -l walltime=59:00

cat $OAR_NODEFILE > $HOME/maquinas.txt
cat $HOME/maquinas.txt
lamboot $HOME/maquinas.txt


RUN=1
while [ $RUN -le 16 ]

do
	
	echo ===============================
	echo ===============================
	echo EXECUTANDO COM $RUN PROCESSOS!!!

	ITER=1

	while [ $ITER -le 20 ]  
	do

		mpirun -np $RUN $HOME/CMP255/simpleraytracerDYNAMIC/src/simplert half.ppm 0 10 0  >> tempo${RUN}.txt

		ITER=$((ITER+1))

	done


	echo FIM DA EXECUCAO
	echo ===============================
	echo ===============================

	RUN=$((RUN+1))

done

lamhalt
