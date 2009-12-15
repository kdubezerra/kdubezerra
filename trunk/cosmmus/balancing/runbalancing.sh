#!/bin/bash

PLAYERS=750
while [ $PLAYERS -le 1500 ]
do
   PROBABILITY=0
	while [ $PROBABILITY -le 90 ]
  	do
      METHOD="AHMED"
      while [ $METHOD != "FIM" ]
      do
      	echo ===============================
      	echo ===============================
      	echo EXECUTANDO COM $PLAYERS PLAYERS, PdeHOTSPOT = ${PROBABILITY} E MÉTODO ${METHOD}!!!
         ./ggp $PLAYERS $PROBABILITY $METHOD > simresults/${PLAYERS}players_${PROBABILITY}hotprobability_${METHOD}.data &

         if [ $METHOD == "AHMED" ]
         then METHOD="PROGREGA"
         else if [ $METHOD == "PROGREGA" ]
              then METHOD="BFBCT"
              else METHOD="FIM"
              fi
         fi         
      done
      if [ $PROBABILITY -eq 0 ]
      then PROBABILITY=70
      else if [ $PROBABILITY -eq 70 ]
           then PROBABILITY=90
           else PROBABILITY=1000
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
