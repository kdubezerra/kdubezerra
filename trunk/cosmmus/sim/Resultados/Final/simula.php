#!/home/po/solon/sys/bin/php -q
<?

// Funcao auxiliar
function tempo(){
    $stimer = explode( ' ', microtime() );
    $stimer = $stimer[1] + $stimer[0];
    return $stimer;
}

// Configuracao
$arquivo_tcl_sem = "freemmg_all-center-sem.tcl";
$arquivo_tcl_com = "freemmg_all-center-com.tcl";
$ns = "/home/po/solon/sys/bin/ns";
$usuarios = 300;
$final = 300;
$repeticoes = 20;

$output = "";

// Timer 1
$timer1 = tempo();

echo " >> $repeticoes simulacoes com $usuarios usuarios\n    Processando...\n";

$pedacos = ceil(($usuarios * 6) / 15);
for($i = 1; $i < $repeticoes; $i++){

    // Timer 2
    $timer2 = tempo();

    $semente = rand(1,100);

    $linha = "$ns $arquivo_tcl_sem $usuarios 6 $pedacos $semente";
    echo "    Executando: $ ns $arquivo_tcl_sem $usuarios 6 $pedacos $semente";

    $output = shell_exec($linha);

    if($i<10){
        $handle = fopen("freemmg_".$usuarios."_".$pedacos."_00".$i."_".$semente."_sem.tr", "w+");
    } else {
        $handle = fopen("freemmg_".$usuarios."_".$pedacos."_0".$i."_".$semente."_sem.tr", "w+");
    }

    fwrite($handle, $output);
    fclose($handle);

    $output = "";

    echo " (Tempo: ".(tempo()-$timer2).")\n";

    $linha = "$ns $arquivo_tcl_com $usuarios 6 $pedacos $semente";
    echo "    Executando: $ ns $arquivo_tcl_com $usuarios 6 $pedacos $semente";

    $output = shell_exec($linha);

    if($i<10){
        $handle = fopen("freemmg_".$usuarios."_".$pedacos."_00".$i."_".$semente."_com.tr", "w+");
    } else {
        $handle = fopen("freemmg_".$usuarios."_".$pedacos."_0".$i."_".$semente."_com.tr", "w+");
    }

    fwrite($handle, $output);
    fclose($handle);

    $output = "";

    echo " (Tempo: ".(tempo()-$timer2).")\n";

}
echo " << Tempo Total: ".(tempo()-$timer1)."\n";
?>
