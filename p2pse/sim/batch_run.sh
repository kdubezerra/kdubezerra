#!/bin/bash

MAX_PLAYERS=200
INC_PLAYERS=40
PLAYERS=40

EXEC_TIME=10


while [[ "$PLAYERS" -le "$MAX_PLAYERS" ]];
do
			
	for USES_AOI in 0 1
	do
		
		for IS_CS in 0 1
		do
		
			#ns cosmmusapp_dyn.tcl $PLAYERS $AOI out_${AOI}_${PLAYERS}.nam graph_${AOI}_${PLAYERS}.tr $EXEC_TIME $GPKT > log_${AOI}_${PLAYERS}_${GPKT}.txt
			
			#puts "$num_players $aoi_type $out_file $graph_in_file $graph_out_file $exec_time $group_pkt $is_cs $uses_aoi"
			#ns ppapp.tcl 1 6 out.nam gri.tr gro.tr 10 0 1 0
			ns ppapp.tcl $PLAYERS 6 out_${PLAYERS}_${IS_CS}_${USES_AOI}.nam graph_${PLAYERS}_${IS_CS}_${USES_AOI}.tr $EXEC_TIME 1 $IS_CS $USES_AOI > log_${PLAYERS}_${IS_CS}_${USES_AOI}.txt
			
			echo 
			echo ==============================================================================
			echo Simulação de $EXEC_TIME segundos, com $PLAYERS jogadores, is_cs[${IS_CS}] e uses_aoi[${USES_AOI}] concluída.		
			echo ==============================================================================
			
		done	
	
	done			  
    	
let	"PLAYERS+=$INC_PLAYERS"	
done	
	

