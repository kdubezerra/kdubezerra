#!/home/po/solon/sys/bin/php -q
<?

// Funcao auxiliar
function tempo(){
    $stimer = explode( ' ', microtime() );
    $stimer = $stimer[1] + $stimer[0];
    return $stimer;
}

// Configuracao
$arquivo_saida = "freemmg_all-center-sem.tcl";
$caminho = "/home/po/solon/p2psim-0.3/example/";
$final = 1000;
$tmatriz = 30;

$output = "";

// Timer 1
$timer1 = tempo();

for($usuarios = 50; $usuarios <= $final; $usuarios+=50){

   $handle = fopen("topology".$usuarios.".txt", "w+");
   for($i = 0; $i <= $usuarios; $i++){

     echo " >> simulacoes com $i usuarios\n    Processando...\n";

     // Timer 2
     $timer2 = tempo();

     $rand1 = rand(0,$tmatriz);
     $rand2 = rand(0,$tmatriz);

     fwrite($handle, $i." ".$rand1." ".$rand2."\n");
   }
   fclose($handle);
   echo " (Tempo: ".(tempo()-$timer2).")\n";
}
echo " << Tempo Total: ".(tempo()-$timer1)."\n";
?>
