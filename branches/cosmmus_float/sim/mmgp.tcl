set ns [new Simulator]

#$ns rtproto DV

$ns color 1 Blue
$ns color 2 Red

set nf [open out.nam w]
$ns namtrace-all $nf

proc finish {} {
	global ns nf
	$ns flush-trace
	close $nf
	exec nam out.nam &
	exit 0
}

#====================
#definindo os objetos


#server node
set server [$ns node]

#internet "cloud"
set cloud [$ns node]

#player nodes
#for {set i 0} {$i < 7} {incr i} {
#  set j($i) [$ns node]
#}

set j0 [$ns node]
set j1 [$ns node]

$ns duplex-link $server $cloud 1Mb 10ms DropTail
$ns duplex-link $j0 $cloud 1Mb 10ms DropTail
$ns duplex-link $j1 $cloud 1Mb 10ms DropTail

#link entre n(i) e n(i+1)
#for {set i 0} {$i < 7} {incr i} {  
#  $ns duplex-link $j($i) $cloud 100Mb 10ms DropTail
#}


#Monitor the queue for the link between node 2 and node 3
$ns duplex-link-op $cloud $server queuePos 0.5


#==============
#comunicando-se

#criando a station do server

#$sstation set class_ 1


#criando as stations dos jogadores
#for {set i 0} {$i < 7} {incr i} {
#	set jstation($i) [new Agent/UDP]
#	#$station($i) set class_ 1
#	$ns attach-agent $j($i) $jstation($i)
#	
#	#agente para comunicar o server com o j_i
#	set sstation($i) [new Agent/UDP]
#	$ns attach-agent $server $sstation($i)
#}

set jstation0 [new Agent/UDP]
$ns attach-agent $j0 $jstation0

set jstation1 [new Agent/UDP]
$ns attach-agent $j1 $jstation1

set sstation [new Agent/UDP]
$ns attach-agent $server $sstation

#Criando uma fonte de tráfego CBR (taxa constante) e associando-a a udp0
#for {set i 0} {$i < 7} {incr i} {
#	set jflow($i) [new Application/Traffic/CBR]
#	$jflow($i) set packetSize_ 30
#	$jflow($i) set interval_ 0.005
#	$jflow($i) attach-agent $jstation($i)
#	
#	set sflow($i) [new Application/Traffic/CBR]
#	$sflow($i) set packetSize_ 210
#	$sflow($i) set interval_ 0.005
#	$sflow($i) attach-agent $sstation($i)
#}

#Criando o agente destino que receberá o tráfego de udp0
#set null0 [new Agent/Null]
#$ns attach-agent $n(3) $null0

#Conectando os agentes
for {set i 0} {$i < 7} {incr i} {
	$ns connect $jstation($i) $sstation($i)
}

#rodando

#Dizendo quando iniciar e quando parar
for {set i 0} {$i < 7} {incr i} {
	$ns at 0.5 "$jflow($i) start"
	#$ns at 0.5 "$sflow($i) start"
}


#$ns rtmodel-at 1.0 down $n(1) $n(2)
#$ns rtmodel-at 2.0 up $n(1) $n(2)

$ns at 5.0 "finish"

$ns run