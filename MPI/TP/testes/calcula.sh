FILE=1
while [ $FILE -le 16 ]

do
	
	echo ===============================
	echo ===============================
	echo CALCULANDO O TEMPO COM $FILE PROCESSOS


	./media_dp tempo${FILE}.txt


	echo FIM DO CALCULO
	echo ===============================
	echo ===============================

	FILE=$((FILE+1))

done
