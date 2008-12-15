#!/bin/bash

MAX_PLAYERS=200
INC_PLAYERS=25
PLAYERS=25

EXEC_TIME=1200


while [[ "$PLAYERS" -le "$MAX_PLAYERS" ]];
do
			
	for AOI in 0
	do
		
			ns cosmmusapp_dyn.tcl $PLAYERS $AOI out_${AOI}_${PLAYERS}_2M.nam graph_${AOI}_${PLAYERS}_2M.tr $EXEC_TIME > log_${AOI}_${PLAYERS}_2M.txt
			echo 
			echo ==============================================================================
			echo Simulação de $EXEC_TIME segundos, com $PLAYERS jogadores e AOI $AOI concluída.		
			echo ==============================================================================
	
	done			  
    	
let	"PLAYERS+=$INC_PLAYERS"	
done	
	

