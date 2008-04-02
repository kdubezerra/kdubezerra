set num_players [lindex $argv 0]
set aoi_type [lindex $argv 1]
set out_file [lindex $argv 2]
set graph_file [lindex $argv 3]
set exec_time [lindex $argv 4]
#set group_pkt [lindex $argv 5]
set group_pkt 0

puts "$num_players $aoi_type $out_file $graph_file $exec_time $group_pkt"

set bw_total 0.0
set bw_max 0.0

#Create a simulator object
set ns [new Simulator]


#Define different colors for data flows
$ns color 1 Blue
$ns color 2 Red

#Open the nam trace file
set nf [open $out_file w]
$ns namtrace-all $nf

set graph [open $graph_file w]

#Define a 'finish' procedure
proc finish {} {
   global ns nf graph graph_file bw_total bw_max exec_time num_players
   $ns flush-trace
	#Close the trace file
   close $nf
   close $graph
	#Execute nam on the trace file
	#exec nam out.nam &
	puts "Average bandwidth: [expr $bw_total/$exec_time]"
	puts "Maximum bandwidth: [expr $bw_max]"
	puts "Estimated maximum needed bandwidth for $num_players players: [expr $bw_max*$num_players] B/s"
	puts "Estimated average needed bandwidth for $num_players players: [expr $bw_total*$num_players/$exec_time] B/s"
   #exec xgraph $graph_file -geometry 800x400 &
   exit 0
}



#Create four nodes
set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]

#Create links between the nodes
$ns duplex-link $n0 $n2 1Mb 10ms DropTail
$ns duplex-link $n1 $n2 1Mb 10ms DropTail
$ns duplex-link $n3 $n2 1Mb 10ms SFQ

$ns duplex-link-op $n0 $n2 orient right-down
$ns duplex-link-op $n1 $n2 orient right-up
$ns duplex-link-op $n2 $n3 orient right

#Monitor the queue for the link between node 2 and node 3
$ns duplex-link-op $n2 $n3 queuePos 0.5
$ns duplex-link-op $n1 $n2 queuePos 0.5
$ns duplex-link-op $n0 $n2 queuePos 0.5

#Create a UDP agent and attach it to node n0
set udp0 [new Agent/UDP]
$udp0 set class_ 1

#$udp0 set packetSize_ 300
#$udp0 set packetSize_ 1
$ns attach-agent $n0 $udp0


set csim [new CosmmusSimulator]
$csim set_num_players $num_players


set csender [new Application/CosmmusApp]
$csender set interval_ 0.25
$csender set packetGrouping_ $group_pkt
#update packet size gotten from yang-yu, 2007
$csender set_packet_size 200
$csender attach-simulator $csim 1
$csender attach-agent $udp0


#Create a Null agent (a traffic sink) and attach it to node n3
set null0 [new Agent/LossMonitor]
$ns attach-agent $n3 $null0


#Connect the traffic sources with the traffic sink
$ns connect $udp0 $null0  



proc record {} {
		global null0 graph bw_total bw_max
		#Get an instance of the simulator
		set ns [Simulator instance]
		#Set the time after which the procedure should be called again
		set time 1.0
			#How many bytes have been received by the traffic sinks?
		set bw0 [$null0 set bytes_]
		set bw_total [expr $bw_total+$bw0]
		if { $bw0 > $bw_max } {
			set bw_max $bw0	
		}
		#Get the current time
		set now [$ns now]
		#Calculate the bandwidth (in MBit/s) and write it to the files
		#puts $graph "$now [expr $bw0/$time*8/1000000]"
		puts $graph "$now [expr $bw0/$time]"
		#Reset the bytes_ values on the traffic sinks
		$null0 set bytes_ 0
		#Re-schedule the procedure
		$ns at [expr $now+$time] "record"
}

#Schedule events for the CBR agents
#Simulator utilizes round AOI
$ns at 0.0 "record"

$ns at 0.1 "$csim start 50 $aoi_type"
$ns at 0.1 "$csender start"

$ns at $exec_time "$csim ppl"

$ns at $exec_time "$csender stop"
$ns at $exec_time "$csim stop"
#Call the finish procedure after 5 seconds of simulation time

$ns at $exec_time "finish"

#Run the simulation
$ns run
