#!/bin/bash

PLAYERS=750
PROBABILITY=90
while [ $PLAYERS -le 1500 ]
do
	while [ $PROBABILITY -le 90 ]
	do
   	echo ===============================
   	echo ===============================
   	echo EXECUTANDO COM $PLAYERS PLAYERS E PdeHOTSPOT = ${PROBABILITY}!!!
      ./main $PLAYERS $PROBABILITY > simresults/kdtree_${PLAYERS}players_${PROBABILITY}hotprobability.data &
      if [ $PROBABILITY -eq 0 ]
      then PROBABILITY=70
      else if [ $PROBABILITY -eq 70 ]
           then PROBABILITY=90
           else PROBABILITY=100
           fi
      fi
	done
	echo FIM DA EXECUCAO
	echo ===============================
	echo ===============================
   if [ $PLAYERS -eq 750 ]
   then PLAYERS=1500
   else PLAYERS=3000
   fi
done
