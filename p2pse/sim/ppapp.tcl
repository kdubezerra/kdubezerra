set num_players [lindex $argv 0]
set aoi_type [lindex $argv 1]
set out_file [lindex $argv 2]
set graph_in_file [lindex $argv 3]
set graph_out_file [lindex $argv 4]
set exec_time [lindex $argv 5]
set group_pkt [lindex $argv 6]
set is_cs [lindex $argv 7]
set uses_aoi [lindex $argv 8]

puts "$num_players $aoi_type $out_file $graph_in_file $graph_out_file $exec_time $group_pkt $is_cs $uses_aoi"
#ns ppapp.tcl 1 6 out.nam gri.tr gro.tr 10 0 1 0
set bi_total 0.0
set bwi_max 0.0
set bo_total 0.0
set bwo_max 0.0

set interval [expr $exec_time/20.0]

#controle do progresso


#Create a simulator object
set ns [new Simulator]


#Define different colors for data flows
$ns color 1 Blue
$ns color 2 Red

#Open the nam trace file
#set nf [open $out_file w]
#$ns namtrace-all $nf

#set graph_in [open $graph_in_file w]
#set graph_out [open $graph_out_file w]


proc record {} {
		#global si_socket ci_socket graph_in graph_out bi_total bwi_max bo_total bwo_max
		global si_socket ci_socket bi_total bwi_max bo_total bwo_max
		#Get an instance of the simulator
		set ns [Simulator instance]
		#Set the time after which the procedure should be called again
		set time 1.0
			#How many bytes have been received by the traffic sinks?
		
		set bi [$si_socket set bytes_]
		set bi_total [expr $bi_total+$bi]
		if { $bi > $bwi_max } {
			set bwi_max $bi
		}
		$si_socket set bytes_ 0
		
		set bo [$ci_socket set bytes_]
		set bo_total [expr $bo_total+$bo]
		if { $bo > $bwo_max } {
			set bwo_max $bo
		}
		$ci_socket set bytes_ 0

		#Get the current time
		set now [$ns now]
		#Calculate the bandwidth (in MBit/s) and write it to the files
		#puts $graph "$now [expr $bw0/$time*8/1000000]"
		#puts $graph_in "$now [expr $bi/$time]"
		#puts $graph_out "$now [expr $bo/$time]"
		#Reset the bytes_ values on the traffic sinks
		
		#Re-schedule the procedure
		$ns at [expr $now+$time] "record"
}

#Define a 'finish' procedure
proc finish {} {
   global ns nf graph_in_file graph_out_file bi_total bwi_max bo_total bwo_max exec_time num_players si_socket ci_socket
   #$ns flush-trace
	#Close the trace file
   #close $nf
   #close $graph_in
   #close $graph_out
	#Execute nam on the trace file
	#exec nam out.nam &
	
	puts "Average server in bandwidth: [expr $bi_total/$exec_time]"
	puts "Maximum server in bandwidth: [expr $bwi_max]"	
	set rcvd_ipkts [$si_socket set npkts_]
	set lost_ipkts [$si_socket set nlost_]
	puts "In-packets lost: $lost_ipkts; packets received: $rcvd_ipkts"
	puts ""
	puts "Average server out bandwidth: [expr $bo_total/$exec_time]"
	puts "Maximum server out bandwidth: [expr $bwo_max]"
	set rcvd_opkts [$ci_socket set npkts_]
	set lost_opkts [$ci_socket set nlost_]
	puts "Out-packets lost: $lost_opkts; packets received: $rcvd_opkts"	
   
   #exec xgraph $graph_file -geometry 800x400 &
   exit 0
}

proc showprogress {} {
	global exec_time interval
	set ns [Simulator instance]
	set now [$ns now]
	set percentage [expr [expr $now/$exec_time]*100]
	puts "$percentage"	
	
	$ns at [expr $now+$interval] "showprogress"
}



#Create four nodes
set s_node [$ns node]
set c_node [$ns node]

#Create links between the nodes
$ns duplex-link $s_node $c_node 1000Mb 100ms SFQ

$ns duplex-link-op $s_node $c_node orient right

#Monitor the queue for the link between node 2 and node 3
$ns duplex-link-op $s_node $c_node queuePos 0.5
#$ns duplex-link-op $co_node $si_node queuePos 0.5

#Create agents for the client and server's in and out sockets
set so_socket [new Agent/UDP]
$so_socket set packetSize_ 655070
$so_socket set class_ 1

set si_socket [new Agent/LossMonitor]
#$si_socket set class_ 1

set co_socket [new Agent/UDP]
$co_socket set packetSize_ 655070
$co_socket set class_ 2

set ci_socket [new Agent/LossMonitor]
#$ci_socket set class_ 1

$ns attach-agent $s_node $so_socket
$ns attach-agent $s_node $si_socket
$ns attach-agent $c_node $co_socket
$ns attach-agent $c_node $ci_socket


set ppsim [new P2pseSimulator]
$ppsim set_client_server $is_cs
$ppsim set_uses_aoi $uses_aoi
$ppsim set_num_players $num_players


set ppserver [new Application/P2pseApp]
$ppserver set interval_ 0.1
$ppserver set packetGrouping_ $group_pkt
$ppserver set pack_aggr_time_ 0.01
#update packet size gotten from yang-yu, 2007
$ppserver set_packet_size 100
$ppserver attach-simulator $ppsim 1 serverapp
$ppserver attach-agent $so_socket

set ppclient [new Application/P2pseApp]
$ppclient set interval_ 0.15
$ppclient set packetGrouping_ $group_pkt
$ppclient set pack_aggr_time_ 0.01
#update packet size gotten from yang-yu, 2007
$ppclient set_packet_size 100
$ppclient attach-simulator $ppsim 1 clientapp
$ppclient attach-agent $co_socket

#Connect the traffic sources with the traffic sink
$ns connect $so_socket $ci_socket
$ns connect $co_socket $si_socket


puts "Setup complete. Initializing simulation..."


#Schedule events for the CBR agents
#Simulator utilizes round AOI
$ns at 0.1 "record"
$ns at 0.1 "showprogress"

$ns at 0.1 "$ppsim start 50 $aoi_type"
$ns at 0.1 "$ppserver start"
$ns at 0.1 "$ppclient start"

$ns at $exec_time "$ppsim ppl"

$ns at $exec_time "$ppserver stop"
$ns at $exec_time "$ppclient stop"
$ns at $exec_time "$ppsim stop"
#Call the finish procedure after 5 seconds of simulation time

$ns at $exec_time "finish"

#Run the simulation
$ns run
