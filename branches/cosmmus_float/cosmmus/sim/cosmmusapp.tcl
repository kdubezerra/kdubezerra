set num_players [$argv(1)]
set aoi_type [$argv(2)]
set out_file [$argv(3)]
set graph_file [$argv(4)]

#Create a simulator object
set ns [new Simulator]


#Define different colors for data flows
$ns color 1 Blue
$ns color 2 Red

#Open the nam trace file
set nf [open out5.nam w]
$ns namtrace-all $nf

set graph [open graph5.tr w]

#Define a 'finish' procedure
proc finish {} {
        global ns nf graph
        $ns flush-trace
	#Close the trace file
        close $nf
        close $graph
	#Execute nam on the trace file
	#exec nam out.nam &
        exec xgraph graph5.tr -geometry 800x400 &
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

#Create a UDP agent and attach it to node n0
set udp0 [new Agent/UDP]
$udp0 set class_ 1
$ns attach-agent $n0 $udp0


set csim [new CosmmusSimulator]
$csim set_num_players 100


set csender [new Application/CosmmusApp]
$csender set interval_ 0.25
$csender set_packet_size 30
$csender attach-simulator $csim 1
$csender attach-agent $udp0


#Create a Null agent (a traffic sink) and attach it to node n3
set null0 [new Agent/LossMonitor]
$ns attach-agent $n3 $null0


#Connect the traffic sources with the traffic sink
$ns connect $udp0 $null0  

proc record {} {
		global null0 graph
		#Get an instance of the simulator
		set ns [Simulator instance]
		#Set the time after which the procedure should be called again
		set time 0.5
			#How many bytes have been received by the traffic sinks?
		set bw0 [$null0 set bytes_]
		#Get the current time
		set now [$ns now]
		#Calculate the bandwidth (in MBit/s) and write it to the files
		#puts $graph "$now [expr $bw0/$time*8/1000000]"
		puts $graph "$now [expr $bw0/$time*8]"
		#Reset the bytes_ values on the traffic sinks
		$null0 set bytes_ 0
		#Re-schedule the procedure
		$ns at [expr $now+$time] "record"
}

#Schedule events for the CBR agents
#Simulator utilizes round AOI
$ns at 0.0 "record"

$ns at 0.1 "$csim start 50 5"
$ns at 0.1 "$csender start"

$ns at 120.0 "$csim ppl"

$ns at 120.5 "$csender stop"
$ns at 120.5 "$csim stop"
#Call the finish procedure after 5 seconds of simulation time

$ns at 120.0 "finish"

#Run the simulation
$ns run
