RANK=4
while [ $RANK -le 1024 ]
do
	echo ===============================
	echo ===============================
	echo CALCULANDO PARA ORDEM ${RANK}!!!
	NP=0
	while [ $NP -le 3 ]
	do
    		NP=$((NP+1))
		echo CALCULANDO A MEDIA E O DESVIO-PADRAO PARA $NP PROCESSADORES E MATRIZ DE ORDEM $RANK...

   		if [ $RANK -eq 768 ]; then
     	 		continue
    		fi
    
    		echo -n ${NP} >> resumo_o${RANK}.txt
		echo -n " " >> resumo_o${RANK}.txt
  		./mdp tempo_p${NP}_o${RANK}.txt 10 >> resumo_o${RANK}.txt
	done
	echo FIM DA EXECUCAO
	echo ===============================
	echo ===============================
 	if [ $RANK -lt 256 ]; then
		RANK=$((RANK*4))
	else
		RANK=$((RANK+256))
	fi
done
