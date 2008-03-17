# Solon Andrade Rabello Junior
# Simulação para publicar artigo do freemmg :)
# ...mas ah!!
# 
#  Restrições
#  Numero de grupos por jogador: Minimo 3 - Maximo 9. Inicial 3
#  
#
#

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

#######################

# traça o dump
#set fi [open out.tr w]
#$ns trace-all $fi
#set nf [open out.nam w]
#$ns namtrace-all $nf
#set f0 [open out0.tr w]
#set f1 [open out1.tr w]
#set f2 [open out2.tr w]
#set f3 [open out3.tr w]
#######################


#######      argumentos                             ############
#argumento 0 = numero inicial de jogadores
#argumento 1 = numero inicial de personagens por jogador
#argumento 2 = numero de pedaços do mundo
#argumento 3 = semente para o randon

proc printargs {args0 args} {
	#puts "o comando foi: $args0"
	#puts "[lindex $args 0] = numero inicial de jogadores"
	#puts "[lindex $args 1] = numero inicial de personagens por jogador"
	#puts "[lindex $args 2] = numero de pedaços do mundo"
	#puts "[lindex $args 3] = semente para o randon"
}


#######      extrutura para o randon                        ###########
#
#
#
#######################################################################
set seed [lindex $argv 3]
expr srand($seed)

proc randomnumber { min max } {
       set val [ expr rand() ]
       set diff [ expr $max - $min ]
       return [ expr $min + round([expr $diff * $val ]) ]
}
#puts "Extrutura Randomica criada com semente [lindex $argv 3]"


#######      cria o mapa                                   ############
# cria o mapa 
#cria uma matriz em forma de [pedaço_mapa][nodos]
#...onde [x][0] é o #+1 de personagens no pedaço
#######################################################################
for {set i 0} {$i < [lindex $argv 2]} {incr i} {
	set mapa($i,0) 1
}
#puts "Mapa Criado com [lindex $argv 2] pedaços"


#######   cria os jogadores/criaturas e espalha no mundo   ############
# cria os jogadores e suas criaturas iniciais
# cria uma matriz [personagem][criaturas]
# ...onde [x][0] diz o numero de criaturas que o personagem controla
# ...joga as criaturas dos personagens randomicamente no mapa
#######################################################################
for {set i 0} {$i < [lindex $argv 0]} {incr i} {
	set personagens($i,0) [lindex $argv 1]
	set aux_pos_mapa [randomnumber 1 [expr [lindex $argv 2] -2] ] 

	#grupo 1
	set aux_prox $mapa([expr $aux_pos_mapa-1],0)
	set personagens($i,1,onde) [expr $aux_pos_mapa-1]
	set personagens($i,1,time) 0
	set mapa([expr $aux_pos_mapa-1],$aux_prox) $i
	incr mapa([expr $aux_pos_mapa-1],0)

	#grupo 1
	set aux_prox $mapa([expr $aux_pos_mapa+1],0)
	set personagens($i,2,onde) [expr $aux_pos_mapa+1]
	set personagens($i,2,time) 0
	set mapa([expr $aux_pos_mapa+1],$aux_prox) $i
	incr mapa([expr $aux_pos_mapa+1],0)

	#grupo 3 
	set aux_prox $mapa($aux_pos_mapa,0)
	set personagens($i,3,onde) $aux_pos_mapa
	set personagens($i,3,time) 0
	set mapa($aux_pos_mapa,$aux_prox) $i
	incr mapa($aux_pos_mapa,0)
	
	#puts "O personagem $i esta com os grupos nos ponto [expr $aux_pos_mapa-1], $aux_pos_mapa e [expr $aux_pos_mapa+1] do mapa"
}

#######      cria nodos                                    ############
# cria um nodo para cada personagem
# cria um array de nodos, de Sinks TCP e de aplicações
#
#######################################################################
for {set i 0} {$i < [lindex $argv 2]} {incr i} {
	set router($i) [$ns node]
}
for {set i 0} {$i < [lindex $argv 0]} {incr i} {
	set n($i) [$ns node]

	#set sink($jogadorA,$jogadorB) [new Agent/TCPSink]
	set sink($i) [new Agent/Null]
	set sink($i) [new Agent/LossMonitor]
	$ns attach-agent $n($i) $sink($i)
}
#puts "Estrutura do mundo criado"

#######      cria controle de conecoes                     ############
#
#######################################################################
for {set i 0} {$i < [lindex $argv 0]} {incr i} {
	set i_aumentado $i
	incr i_aumentado
	for {set f $i_aumentado} {$f < [lindex $argv 0]} {incr f} {
		set conex($i,$f) 0
		set conex($i,$f,j) 0
	}
}


#######      cria as conecoes                              ############
# Cria as conecoes e nao armazena nada sobre elas...legal né?!! :)
# O controle é feito apartir da meia matriz "conex" 
# ...conex(jogadorA,jogadorB) onde jogadorA<jogadorB
#######################################################################
# ..para cada ponto do mapa
for {set i 0} {$i < [lindex $argv 2]} {incr i} {
	#puts ""
	#puts "############ Parte $i do mapa  ############"
	# ..para cada nodo desse ponto
	for {set j 1} {$j < $mapa($i,0)} {incr j} {
		# ..cria conecao com cada outra criatura desse ponto
		for {set h 1} {$h < $mapa($i,0)} {incr h} {
			# ..se nodoA < nodo B
			if {$mapa($i,$j) < $mapa($i,$h)} {
				# ..se nao existir conecao entre eles ainda
				if {$conex($mapa($i,$j),$mapa($i,$h)) == 0} {
			
					# ..cria a extrutra tcp A
					#set tcp($mapa($i,$j),$mapa($i,$h)) [new Agent/TCP]
					set tcp($mapa($i,$j),$mapa($i,$h)) [new Agent/UDP]
					$tcp($mapa($i,$j),$mapa($i,$h)) set class_ $i
					$ns attach-agent $n($mapa($i,$j)) $tcp($mapa($i,$j),$mapa($i,$h))

					# ..cria a extrutra tcp B
					#set tcp($mapa($i,$h),$mapa($i,$j)) [new Agent/TCP]
					set tcp($mapa($i,$h),$mapa($i,$j)) [new Agent/UDP]
					$tcp($mapa($i,$h),$mapa($i,$j)) set class_ $i
					$ns attach-agent $n($mapa($i,$h)) $tcp($mapa($i,$h),$mapa($i,$j))
					
					# ..cria conecao propriamente dita
					$ns duplex-link $n($mapa($i,$j)) $router($i) 1Gb 1ms DropTail
					$ns duplex-link $n($mapa($i,$h)) $router($i) 1Gb 1ms DropTail
					$ns queue-limit $n($mapa($i,$j)) $router($i) 1000			
					$ns queue-limit $n($mapa($i,$h)) $router($i) 1000
					$ns duplex-link-op $n($mapa($i,$j)) $router($i) queuePos 0.5
					$ns duplex-link-op $n($mapa($i,$h)) $router($i) queuePos 0.5
					$ns connect $tcp($mapa($i,$j),$mapa($i,$h)) $sink($mapa($i,$j))
					$ns connect $tcp($mapa($i,$h),$mapa($i,$j)) $sink($mapa($i,$h)) 
					
					# ..cria os agentes de transmissão A e B
					set tcp_($mapa($i,$j),$mapa($i,$h)) [new Application/Traffic/CBR]
					set tcp_($mapa($i,$h),$mapa($i,$j)) [new Application/Traffic/CBR]
					$tcp_($mapa($i,$j),$mapa($i,$h)) set packetSize_ 60
					$tcp_($mapa($i,$h),$mapa($i,$j)) set packetSize_ 60
					$tcp_($mapa($i,$j),$mapa($i,$h)) set interval_ 0.2
					$tcp_($mapa($i,$h),$mapa($i,$j)) set interval_ 0.2
					$tcp_($mapa($i,$j),$mapa($i,$h)) attach-agent $tcp($mapa($i,$j),$mapa($i,$h))
					$tcp_($mapa($i,$h),$mapa($i,$j)) attach-agent $tcp($mapa($i,$h),$mapa($i,$j))
					
					# ..ajusta o controle de coneções
					incr conex($mapa($i,$j),$mapa($i,$h))
					set conex($mapa($i,$j),$mapa($i,$h),j) 1
					#puts "   No ponto $i do mapa, foi criada conecao entre os personagens $mapa($i,$j) == $mapa($i,$h)"	
				} else {
					# ..ajusta o controle de coneções
					incr conex($mapa($i,$j),$mapa($i,$h))
					#puts "   No ponto $i do mapa, foi incr a conex entre os $mapa($i,$j) == $mapa($i,$h)"
				}
			} else {
			} 
		}
	}
}
#puts "conexoes criadas"

####### ve se o ponto esta livre                           ############
# retorna 0: se livre
# retorna 1: se ocupado
#######################################################################
proc ponto_livre { dono onde } {

	global argv
	global personagens
	
	if { $onde == -1 || $onde == [lindex $argv 2] } {
		return 1
	} else {
		for {set i 1} { $i <= $personagens($dono,0) } {incr i} {
			if { $personagens($dono,$i,onde) == $onde } {
				return 1
			} else {
			}
		}
		return 0
	}
}

proc cria_conecao { dono onde} {

	global ns n tcp tcp_ sink router mapa conex

	# ..para todos os jogadores naquele ponto do mapa, menos o ultimo que acabou de entrar
	for {set h 1} {$h < [expr $mapa($onde,0) - 1]} {incr h} {
		# ..se jogadorA < jogadorB
		if {$dono < $mapa($onde,$h)} {
			set jogadorA $dono
			set jogadorB $mapa($onde,$h)
		} else {
			set jogadorA $mapa($onde,$h)
			set jogadorB $dono
		}
		# ..se nao existir conecao entre eles ainda
		if {$conex($jogadorA,$jogadorB) == 0} {
			if { $conex($jogadorA,$jogadorB,j) == 0} {
				# ..cria a extrutra tcp A
				#set tcp($jogadorA,$jogadorB) [new Agent/TCP]
				set tcp($jogadorA,$jogadorB) [new Agent/UDP]
				#$tcp($jogadorA,$jogadorB) set class_ $onde
				#set sink($jogadorB) [new Agent/TCPSink]
				$ns attach-agent $n($jogadorA) $tcp($jogadorA,$jogadorB)
				# ..cria a extrutra tcp B
				#set tcp($jogadorB,$jogadorA) [new Agent/TCP]
				set tcp($jogadorB,$jogadorA) [new Agent/UDP]
				#$tcp($jogadorB,$jogadorA) set class_ $onde
				$ns attach-agent $n($jogadorB) $tcp($jogadorB,$jogadorA)
			
				# ..cria conecao propriamente dita
				$ns duplex-link $n($jogadorA) $router($onde) 1Gb 1ms DropTail
				$ns duplex-link $n($jogadorB) $router($onde) 1Gb 1ms DropTail
				$ns queue-limit $n($jogadorA) $router($onde) 1000			
				$ns queue-limit $n($jogadorB) $router($onde) 1000
				$ns duplex-link-op $n($jogadorA) $router($onde) queuePos 0.5
				$ns duplex-link-op $n($jogadorB) $router($onde) queuePos 0.5
				$ns connect $tcp($jogadorA,$jogadorB) $sink($jogadorA)
				$ns connect $tcp($jogadorB,$jogadorA) $sink($jogadorB) 
				
				# ..cria os agentes de transmissão A e B
				set tcp_($jogadorA,$jogadorB) [new Application/Traffic/CBR]
				set tcp_($jogadorB,$jogadorA) [new Application/Traffic/CBR]
				$tcp_($jogadorA,$jogadorB) set packetSize_ 60
				$tcp_($jogadorB,$jogadorA) set packetSize_ 60
				$tcp_($jogadorA,$jogadorB) set interval_ 0.2
				$tcp_($jogadorB,$jogadorA) set interval_ 0.2
				$tcp_($jogadorA,$jogadorB) attach-agent $tcp($jogadorA,$jogadorB)
				$tcp_($jogadorB,$jogadorA) attach-agent $tcp($jogadorB,$jogadorA)
				
				# ..ajusta o controle de coneções
				incr conex($jogadorA,$jogadorB)
				#puts "   No ponto $onde do mapa, foi criada conecao entre os personagens $jogadorA == $jogadorB"
				#puts "     Inicio no momento $quando a transmissão do Jogador $jogadorA para o $jogadorB"
				set now [$ns now]
				$ns at $now "$tcp_($jogadorA,$jogadorB) start"
				$ns at $now "$tcp_($jogadorB,$jogadorA) start"
			} else {
				set now [$ns now]
				incr conex($jogadorA,$jogadorB)
				$ns at $now "$tcp_($jogadorA,$jogadorB) start"
				$ns at $now "$tcp_($jogadorB,$jogadorA) start"
			}
		} else {
			# ..ajusta o controle de coneções
			incr conex($jogadorA,$jogadorB)
			#puts "   No ponto $onde do mapa, foi incr a conex entre os $jogadorA == $jogadorB"
		}
	}
}


#######      cria e coloca uma criatura no mundo           ############
# 
#  
# 
#######################################################################
proc cria_coloca_criatura_no_mundo { dono criatura quando } {

	global personagens mapa ns

	set direita [ponto_livre $dono [expr $personagens($dono,$criatura,onde) + 1] ]
	set esquerda [ponto_livre $dono [expr $personagens($dono,$criatura,onde) - 1] ]
	
	if {$esquerda == 1} {
       		if { $direita == 0 && $personagens($dono,0) < 9 } {
			set aux_pos_mapa [expr $personagens($dono,$criatura,onde) + 1]
		} else {
			#puts "   Não pode criar. A criatura $criatura está presa."
			return
		}
	} else {
		if { $direita == 1 && $personagens($dono,0) < 9 } {
			set aux_pos_mapa [expr $personagens($dono,$criatura,onde) - 1]
		} else {
			if { [randomnumber 0 1] == 0 } {
				if { $personagens($dono,0) < 9 } {
					set aux_pos_mapa [expr $personagens($dono,$criatura,onde) - 1]		
				} else {
					#puts "   Não pode criar. Já possui 9 criaturas."
					return
				}	
			} else {
				if { $personagens($dono,0) < 9 } {
					set aux_pos_mapa [expr $personagens($dono,$criatura,onde) + 1]
				} else {
					#puts "   Não pode criar. Já possui 9 criaturas."
					return
				}	
			}
		}
	}
	set mapa($aux_pos_mapa,$mapa($aux_pos_mapa,0)) $dono
	incr mapa($aux_pos_mapa,0)
	incr personagens($dono,0)
	set personagens($dono,$personagens($dono,0),onde) $aux_pos_mapa
	set personagens($dono,$personagens($dono,0),time) [$ns now]
	cria_conecao $dono $aux_pos_mapa
	#puts "   Criatura $criatura pos $personagens($dono,$criatura,onde). Nova criatura $personagens($dono,0) pos $aux_pos_mapa"
}

#######      para a conecao entre dois jogadores           ############
# ...para a conexao entre os dois jogadores
# e atualiza a matriz conex e a mapa
# 
#######################################################################
proc stop_conecao { dono onde } {

	global conex mapa ns tcp_

	for {set h 1} {$h < $mapa($onde,0)} {incr h} {
		if {$dono == $mapa($onde,$h)} {
			set mapa($onde,0) [expr $mapa($onde,0) - 1]
			set mapa($onde,$h) $mapa($onde,$mapa($onde,0))
		} else {
		}
	}		
	for {set h 1} {$h < $mapa($onde,0)} {incr h} {
		# ..se jogadorA < jogadorB
		if {$dono < $mapa($onde,$h)} {
			# .. testa se existe mais conexoes entre eles
			if {$conex($dono,$mapa($onde,$h)) == 1} {
				set conex($dono,$mapa($onde,$h)) [expr $conex($dono,$mapa($onde,$h)) -1]
				$ns at [$ns now] "$tcp_($dono,$mapa($onde,$h)) stop"
				$ns at [$ns now] "$tcp_($mapa($onde,$h),$dono) stop"
			} else {
				set conex($dono,$mapa($onde,$h)) [expr $conex($dono,$mapa($onde,$h)) -1]
			}
		} else {
			if {$conex($mapa($onde,$h),$dono) == 1} {
				set conex($mapa($onde,$h),$dono) [expr $conex($mapa($onde,$h),$dono) -1]
				$ns at [$ns now] "$tcp_($dono,$mapa($onde,$h)) stop"
				$ns at [$ns now] "$tcp_($mapa($onde,$h),$dono) stop"
			} else {
				set conex($mapa($onde,$h),$dono) [expr $conex($mapa($onde,$h),$dono) -1]
			}
		}
		#puts "   No ponto $onde do mapa, foi parada a transferencia entre os personagens $jogadorA == $jogadorB"
		#puts "   Parou no momento $quando a transmissão do Jogador $jogadorA para o $jogadorB"
	}
	#puts ""
}

#######      elimina uma criatura do jogador no mundo           ############
# ...funcao chamada para eliminar uma criatura de um jogador, se possivel,
# tambem atualiza a matriz personagens e utiliza a funcao stop_conexao 
# 
#######################################################################
proc elimina_criatura { dono criatura } {

	global personagens

	if { $personagens($dono,0) > 3 } {
		# ordena
		stop_conecao $dono $personagens($dono,$criatura,onde)
		set personagens($dono,$criatura,onde) $personagens($dono,$personagens($dono,0),onde)
		set personagens($dono,$criatura,time) $personagens($dono,$personagens($dono,0),time)
		set personagens($dono,0) [expr $personagens($dono,0) - 1]
		
	} else {
		#puts "   Não pode morrer. Possui somente 3 criaturas."
	} 
}


##########################
proc record {} {
        global sink argv mapa personagens conex
        #Get an instance of the simulator
        set ns [Simulator instance]
        #Set the time after which the procedure should be called again
        set time 1
	
        #How many bytes have been received by the traffic sinks?
        #Calculate the bandwidth (in MBit/s) and write it to the files        
	set media_bw 0
	set soma_bw 0
	set maior_bw 0
	for {set h 0} {$h < [lindex $argv 0]} {incr h} {
		set bw($h) [$sink($h) set bytes_]
		set soma_bw [expr $soma_bw+$bw($h)]	
		if { $maior_bw < $bw($h) } {
			set maior_bw $bw($h)
		} else {
		}
	}
	set media_bw [expr $soma_bw/[lindex $argv 0]]
	
	set now_inteiro [expr round([$ns now]) + 0]
	puts "$now_inteiro $media_bw $maior_bw"

#	for {set h 0} {$h < [lindex $argv 2]} {incr h} {
#		puts $f1 ""
#		puts $f1 "============  $now_inteiro $h  =============="
#			puts $f1 "$mapa($h,0)"	
#		for {set j 1} {$j < $mapa($h,0)} {incr j} {
#			puts $f1 "   $mapa($h,$j)"
#		}
#	}
#	for {set h 0} {$h < [lindex $argv 0]} {incr h} {
#		puts $f2 ""
#		puts $f2 "============  $now_inteiro $h  =============="
#		puts $f2 "$personagens($h,0)"	
#		for {set j 1} {$j <= $personagens($h,0)} {incr j} {
#			puts $f2 "   $personagens($h,$j,onde)"
#		}
#	}
#	for {set h 0} {$h < [lindex $argv 0]} {incr h} {
#		puts $f3 ""
#		puts $f3 "============  $now_inteiro $h  =============="			
#		for {set j [expr $h+1]} {$j < [lindex $argv 0]} {incr j} {
#			puts $f3 "Conexao $h == $j =  $conex($h,$j)"
#		}
#	}
	##puts $f0 "$now [expr $bw0/$time*8/1000000]"
        ##puts $f1 "$now [expr $bw1/$time*8/1000000]"
        ##puts $f2 "$now [expr $bw2/$time*8/1000000]"

	#Reset the bytes_ values on the traffic sinks
        for {set h 0} {$h < [lindex $argv 0]} {incr h} {
		$sink($h) set bytes_ 0
	}
        #Re-schedule the procedure
        $ns at [expr [$ns now]+$time] "record"
}


#######      mostra um ping na tela                        ############
# ...sem misterio...
# mostra o segundo atual a cada segundo de simulação 
# 
#######################################################################
proc ping {} {
	set ns [Simulator instance]
	puts "[$ns now]"
	$ns at [expr [$ns now]+1] "ping"
}



#######      simulacao propriamente dita                   ############
# simula as possiveis ações dos jogadores, escolhem randomicamente
# uma criatura dele para executar uma ação também randomica, e faz  
# a otimizacao de trafego, caso descomentado último trecho
#######################################################################
proc simulacao {} {
	set ns [Simulator instance]
	global argv personagens mapa tcp_
	set now [expr int([$ns now])]

	#..para cada jogador
	for {set i 0} {$i < [lindex $argv 0]} {incr i} {
		#..sorteia a criatura por nivel de atencao
		set rand_quem [randomnumber 1 100]
		if {$rand_quem <= 90} {
			#..escolhe a sorteada
			if {$personagens($i,0) <= 3} {	
				set rand_quem_precisamente 1
			} elseif {$personagens($i,0) <= 6} {
				set rand_aux [randomnumber 1 2]
				set rand_quem_precisamente [expr [expr $rand_aux * 3] - 2]
			} else {
				set rand_aux [randomnumber 1 3]
				set rand_quem_precisamente [expr [expr $rand_aux * 3] - 2]
			}
		} elseif {$rand_quem <= 99} {
			#..escolhe a sorteada dentre as do mesmo nivel de atencao
			if {$personagens($i,0) <= 4} {	
				set rand_quem_precisamente 2
			} elseif {$personagens($i,0) <= 7} {
				set rand_aux [randomnumber 1 2]
				set rand_quem_precisamente [expr [expr $rand_aux * 3] -1]
			} else {
				set rand_aux [randomnumber 1 3]
				set rand_quem_precisamente [expr [expr $rand_aux * 3] -1]
			}
		} else {
			#..escolhe a sorteada dentre as do mesmo nivel de atencao
			if {$personagens($i,0) <= 5} {	
				set rand_quem_precisamente 3
			} elseif {$personagens($i,0) <= 8} {
				set rand_aux [randomnumber 1 2]
				set rand_quem_precisamente [expr $rand_aux * 3]
		} else {
				set rand_aux [randomnumber 1 3]
				set rand_quem_precisamente [expr $rand_aux * 3]
			} 
		}
		#..sorteia a ação a ser executada
		set rand_oque [randomnumber 1 100]
		if {$rand_oque <= 85} {
			#..sorteou a ação toque
			#puts "Segundo $now: player $i = Tocou; Criatura $rand_quem_precisamente"
			set personagens($i,$rand_quem_precisamente,time) $now
		} elseif {$rand_oque <= 90} {
			#..sorteou a ação cria
			#puts "Segundo $now: Player $i = Cria; Criatura $rand_quem_precisamente pos $personagens($i,$rand_quem_precisamente,onde)"
			set personagens($i,$rand_quem_precisamente,time) $now
			cria_coloca_criatura_no_mundo $i $rand_quem_precisamente $now
			set personagens($i,$personagens($i,0),time) $now
		} elseif {$rand_oque <= 95} {
			#..sorteou a ação mata
			#puts "Segundo $now: Player $i = Morte; Criatura $rand_quem_precisamente"
			elimina_criatura $i $rand_quem_precisamente
			set personagens($i,$personagens($i,0),time) $now
		} else {
			#..sorteou a ação move
			#puts "Segundo $now: Player $i = Moveu; Criatura $rand_quem_precisamente. Onde $personagens($i,$rand_quem_precisamente,onde) + / - 1"
		}
	}

#	for {set i 0} {$i < [lindex $argv 0]} {incr i} {
#		for {set h 1} {$h < $personagens($i,0)} {incr h} {
#			if { [expr $now - $personagens($i,$h,time)] >= 30 } {
#				set posicao $personagens($i,$h,onde)
#				for {set x 1} {$x < $mapa($posicao,0) } {incr x} {
#				    if { $mapa($posicao,$x) != $i && [$tcp_($i,$mapa($posicao,$x)) set interval_] != 1.0 } {
#					       #puts " Mudei o tempo do $i para $mapa($posicao,$x) no nodo $posicao para 1.0"
#					       $tcp_($i,$mapa($posicao,$x)) set interval_ 1.0
#				    } else {
#					   #puts " NAO Mudou o tempo do $i para $mapa($posicao,$x) no nodo $posicao para 1.0"
#				    }	
#				}
#			} elseif { [expr $now - $personagens($i,$h,time)] >= 10 } {
#				set posicao $personagens($i,$h,onde)
#				for {set x 1} {$x < $mapa($posicao,0) } {incr x} {
#				    if { $mapa($posicao,$x) != $i && [$tcp_($i,$mapa($posicao,$x)) set interval_] != 0.4 } {
#					       #puts " Mudei o tempo do $i para $mapa($posicao,$x) no nodo $posicao para 0.4"
#					       $tcp_($i,$mapa($posicao,$x)) set interval_ 0.4
#				    } else {
#					   #puts " NAO Mudou o tempo do $i para $mapa($posicao,$x) no nodo $posicao para 0.4"
#				    }	
#				}
#			} else {
#				set posicao $personagens($i,$h,onde)
#				for {set x 1} {$x < $mapa($posicao,0) } {incr x} {
#				    if { $mapa($posicao,$x) != $i && [$tcp_($i,$mapa($posicao,$x)) set interval_] != 0.2 } {
#					       #puts " Mudei o tempo do $i para $mapa($posicao,$x) no nodo $posicao para 0.2"
#					       $tcp_($i,$mapa($posicao,$x)) set interval_ 0.2
#				    } else {
#					   #puts " NAO Mudou o tempo do $i para $mapa($posicao,$x) no nodo $posicao para 0.2"
#				    }	
#				}
#			}
#		}
#	}
	$ns at [expr $now + 1] "simulacao"
}

#######################

#puts "Começa a disparar os pacotes"
for {set i 0} {$i < [lindex $argv 0]} {incr i} {
	set i_aumentado $i
	incr i_aumentado
	for {set f $i_aumentado} {$f < [lindex $argv 0]} {incr f} {
		if {$conex($i,$f) != 0} {
			#puts "Inicio de transmissão do Jogador $i para o $f"
			$ns at 0.001 "$tcp_($i,$f) start"
			$ns at 0.001 "$tcp_($f,$i) start"
		} else {
		}
	}
}
#$ns at 0.0 "ping"
$ns at 0.01 "simulacao"
$ns at 0.99 "record"
$ns at 1800.0 "finish"


#######################

proc finish {} {  
	#puts "Tô finalizando..."
	#global ns fi nf
	
	#global f0 f1 f2 f3
        #Close the output files
        #close $f0
	#close $f1
	#close $f2
	#close $f3
	
	##puts "   vo faze o flush-trace"
	#$ns flush-trace
	##puts "   to fechando o fi"	
	#close $fi
	##puts "   to fechando o nf"	
	#close $nf

	##puts "running nam..."
	#exec nam out.nam
	exit 0
}



$ns run
