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
#    echo ${NP} >> resumo_it_t${NP}.txt
    ./mdp tempo_it_t${NP}_o${RANK}.txt 10 >> resumo_it_o${RANK}.txt

   	if [ $RANK -eq 768 ]; then
      continue
    fi
    
#    echo ${NP} >> resumo_rec_t${NP}.txt
  	./mdp tempo_rec_t${NP}_o${RANK}.txt 10 >> resumo_rec_o${RANK}.txt
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
