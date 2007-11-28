P=1
while [ $P -le 25 ]

do
	
	echo ===============================
	echo Calculando o tempo com $P processos

	N=2
	while [ $N -le 1024 ]
		do

		echo Calculando o tempo com $P processos, entrada de tamanho $N		

		./media_dp tempo_${P}_${N}.txt 10 >> experiment_${P}.txt

		N=$((N*2))
		done

	echo FIM DO CALCULO

	P=$((P+6))

done
