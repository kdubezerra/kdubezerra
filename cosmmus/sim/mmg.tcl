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


#servidor node
set servidor [$ns node]

#internet "cloud"
set cloud [$ns node]

#player nodes
for {set i 0} {$i < 7} {incr i} {
  set j($i) [$ns node]
}

$ns duplex-link $servidor $cloud 1Mb 10ms DropTail

#link entre n(i) e n(i+1)
for {set i 0} {$i < 7} {incr i} {
  #$ns duplex-link $j($i) $j([expr ($i+1)%7]) 1Mb 10ms DropTail  
  $ns duplex-link $j($i) $cloud 1Mb 10ms DropTail
}


#Monitor the queue for the link between node 2 and node 3
$ns duplex-link-op $cloud $servidor queuePos 0.5


#==============
#comunicando-se

#criando a station do servidor

#$sstation set class_ 1


#criando as stations dos jogadores
for {set i 0} {$i < 7} {incr i} {
	set jsender($i) [new Agent/UDP]
	set jrecver($i) [new Agent/Null]
	#$station($i) set class_ 1
	$ns attach-agent $j($i) $jsender($i)
	$ns attach-agent $j($i) $jrecver($i)
	
	#agente para comunicar o servidor com o j_i
	set ssender($i) [new Agent/UDP]
	set srecver    [new Agent/Null]
	$ns attach-agent $servidor $ssender($i)
	$ns attach-agent $servidor $srecver
}

#Criando uma fonte de tráfego CBR (taxa constante) e associando-a a udp0
for {set i 0} {$i < 7} {incr i} {
	set jflow($i) [new Application/Traffic/CBR]
	$jflow($i) set packetSize_ 30
	$jflow($i) set interval_ 0.005
	$jflow($i) attach-agent $jsender($i)
	
	set sflow($i) [new Application/Traffic/CBR]
	$sflow($i) set packetSize_ 210
	$sflow($i) set interval_ 0.005
	$sflow($i) attach-agent $ssender($i)
}

#Criando o agente destino que receberá o tráfego de udp0
#set null0 [new Agent/Null]
#$ns attach-agent $n(3) $null0

#Conectando os agentes
for {set i 0} {$i < 7} {incr i} {
	$ns connect $jsender($i) $srecver
	$ns connect $ssender($i) $jrecver($i)
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
