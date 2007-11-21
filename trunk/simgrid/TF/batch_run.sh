#!/bin/bash

MAX_TASK_SIZE=45000000
INC_TASK=3000000
TASK_SIZE=500000

MAX_COMM_SIZE=450
INC_COMM=30
COMM_SIZE=1

MAX_PLAT_SIZE=91
INC_PLAT=6
PLAT_SIZE=1


while [[ "$TASK_SIZE" -le "$MAX_TASK_SIZE" ]];
do

	while [[ "$COMM_SIZE" -le "$MAX_COMM_SIZE" ]];
	do
	
		while [[ "$PLAT_SIZE" -le "$MAX_PLAT_SIZE" ]];
		do
			
			for SCHED in RR DYN HEAP
			do
				./fcpc $PLAT_SIZE 5000 $SCHED $TASK_SIZE $COMM_SIZE
				echo $PLAT_SIZE $TASK_SIZE $COMM_SIZE $SCHED
				./sched fc_plat.xml fc_dep_${SCHED}.xml
			done			  
  
  		let "PLAT_SIZE+=$INC_PLAT"
  	done
  	
  	let	"COMM_SIZE+=$INC_COMM"	
	done  	
	
	let "TASK_SIZE+=$INC_TASK"
done           # No surprises, so far.
