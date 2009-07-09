RUN=1
while [ $RUN -le 4 ]
do
	echo ===============================
	echo ===============================
	echo EXECUTANDO COM $RUN PROCESSOS!!!
	RANK=4
	while [ $RANK -le 1024 ]
	do
		echo CALCULANDO A MEDIA E O DESVIO-PADRAO PARA $RUN PROCESSADORES E MATRIZ DE ORDEM $RANK...
		./mdp tempo_p${RUN}_o${RANK}.txt 10 >> resumo_p${RUN}.txt
		if [ $RANK -lt 256 ]; then			
			RANK=$((RANK*4))
		else
			RANK=$((RANK*2))
		fi
	done
	echo FIM DA EXECUCAO
	echo ===============================
	echo ===============================
	RUN=$((RUN+1))
done
