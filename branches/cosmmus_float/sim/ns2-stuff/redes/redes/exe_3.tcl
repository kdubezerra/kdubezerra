
# dormento e seu madruga eh mal (c)2004

set ns [new Simulator]

$ns color 0 blue
$ns color 1 red
$ns color 2 green
$ns color 3 yellow
$ns color 4 black
$ns color 5 white
$ns color 6 pink
$ns color 7 brown
$ns color 8 black
$ns color 9 white
$ns color 10 pink
$ns color 11 brown





set t1 [$ns node]
set t2 [$ns node]
set t3 [$ns node]
set t4 [$ns node]

set r1 [$ns node]
set r2 [$ns node]
set r3 [$ns node]
set r4 [$ns node]

set a1 [$ns node]
set a2 [$ns node]
set a3 [$ns node]
set a4 [$ns node]



# traça o dump
set f [open out.tr w]
$ns trace-all $f
set nf [open out.nam w]
$ns namtrace-all $nf




$ns duplex-link $t1 $a1 2Mb 1ms DropTail
$ns duplex-link $t2 $a1 2Mb 1ms DropTail
$ns duplex-link $t3 $a1 2Mb 1ms DropTail
$ns duplex-link $t4 $a1 2Mb 1ms DropTail
$ns queue-limit $t1 $a1 10
$ns queue-limit $t2 $a1 10
$ns queue-limit $t3 $a1 10
$ns queue-limit $t4 $a1 10
$ns duplex-link-op $t1 $a1 queuePos 0.5
$ns duplex-link-op $t2 $a1 queuePos 0.5
$ns duplex-link-op $t3 $a1 queuePos 0.5
$ns duplex-link-op $t4 $a1 queuePos 0.5


$ns duplex-link $a1 $a2 1Mb 1ms DropTail
$ns duplex-link $a2 $a3 0.5Mb 1ms DropTail
$ns duplex-link $a3 $a4 0.1Mb 1ms DropTail
$ns queue-limit $a1 $a2 10
$ns queue-limit $a2 $a3 10
$ns queue-limit $a3 $a4 10
$ns duplex-link-op $a1 $a2 queuePos 0.5
$ns duplex-link-op $a2 $a3 queuePos 0.5
$ns duplex-link-op $a3 $a4 queuePos 0.5


$ns duplex-link $a1 $r1 2Mb 1ms DropTail
$ns duplex-link $a2 $r2 1Mb 1ms DropTail
$ns duplex-link $a3 $r3 0.5Mb 1ms DropTail
$ns duplex-link $a4 $r4 0.1Mb 1ms DropTail
$ns queue-limit $a1 $r1 10
$ns queue-limit $a2 $r2 10
$ns queue-limit $a3 $r3 10
$ns queue-limit $a4 $r4 10
$ns duplex-link-op $a1 $r1 queuePos 0.5
$ns duplex-link-op $a2 $r2 queuePos 0.5
$ns duplex-link-op $a3 $r3 queuePos 0.5
$ns duplex-link-op $a4 $r4 queuePos 0.5


set tcp1 [new Agent/TCP]
set tcp2 [new Agent/TCP]
set tcp3 [new Agent/TCP]
set tcp4 [new Agent/TCP]
$tcp1 set class_ 0
$tcp2 set class_ 1
$tcp3 set class_ 2
$tcp4 set class_ 3




set sink1 [new Agent/TCPSink]
set sink2 [new Agent/TCPSink]
set sink3 [new Agent/TCPSink]
set sink4 [new Agent/TCPSink]


$ns attach-agent $t1 $tcp1
$ns attach-agent $t2 $tcp2
$ns attach-agent $t3 $tcp3
$ns attach-agent $t4 $tcp4
$ns attach-agent $r1 $sink1
$ns attach-agent $r2 $sink2
$ns attach-agent $r3 $sink3
$ns attach-agent $r4 $sink4


$ns connect $tcp1 $sink1
$ns connect $tcp2 $sink2
$ns connect $tcp3 $sink3
$ns connect $tcp4 $sink4


#########################


set tcp_1 [new Application/FTP]
$tcp1 set packetSize_ 1000
set tcp_2 [new Application/FTP]
$tcp2 set packetSize_ 1000
set tcp_3 [new Application/FTP]
$tcp3 set packetSize_ 1000
set tcp_4 [new Application/FTP]
$tcp4 set packetSize_ 1000

##########################

$tcp_1 attach-agent $tcp1
$tcp_2 attach-agent $tcp2
$tcp_3 attach-agent $tcp3
$tcp_4 attach-agent $tcp4

#######################
#mudei os tempos

$ns at 0.0 "$tcp_1 start"
$ns at 0.1 "$tcp_2 start"
$ns at 0.2 "$tcp_3 start"
#$ns at 0.3 "$tcp_4 start"

$ns at 10.0 "finish"

#######################

proc finish {} {
	global ns f nf
	$ns flush-trace
	close $f
	close $nf

	puts "running nam..."
	exec nam out.nam &
	exit 0
}

$ns run



