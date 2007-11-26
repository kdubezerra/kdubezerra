export PATH=/home/gppd/cebbezerra/my_mpi/install_dir/bin/:$PATH

#OAR -l walltime=59:00

cat $OAR_NODEFILE > $HOME/maquinas.txt
cat $HOME/maquinas.txt
lamboot $HOME/maquinas.txt

LENGTH=0
while [ $LENGTH -le 1000000 ]
do
	echo ===============================
	echo Testando o envio de $LENGTH numeros de ponto flutuante
	ITER=1
	while [ $ITER -le 5 ]  
	do
		echo Enviando $LENGTH numeros de ponto flutuante, iteracao $ITER
		mpirun -np 3 $HOME/svn/trunk/MPI/benchmarks/pp $LENGTH 1 >> tempo_${LENGTH}.txt
		ITER=$((ITER+1))
	done
	while [ $ITER -le 10 ]  
	do
		echo Enviando $LENGTH numeros de ponto flutuante, iteracao $ITER
		mpirun -np 3 $HOME/svn/trunk/MPI/benchmarks/pp $LENGTH 2 >> tempo_${LENGTH}.txt
		ITER=$((ITER+1))
	done
	echo FIM DA EXECUCAO
	echo ===============================
	echo .
	if [ $LENGTH -eq 0 ]
	then
		LENGTH=1
	else
		LENGTH=$((LENGTH*10))
	fi
done
lamhalt
