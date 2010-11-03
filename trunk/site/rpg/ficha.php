 
<script language="JavaScript">
<!--
function MM_swapImgRestore() { //v3.0
  var i,x,a=document.MM_sr; for(i=0;a&&i<a.length&&(x=a[i])&&x.oSrc;i++) x.src=x.oSrc;
}

function MM_preloadImages() { //v3.0
  var d=document; if(d.images){ if(!d.MM_p) d.MM_p=new Array();
    var i,j=d.MM_p.length,a=MM_preloadImages.arguments; for(i=0; i<a.length; i++)
    if (a[i].indexOf("#")!=0){ d.MM_p[j]=new Image; d.MM_p[j++].src=a[i];}}
}

function MM_findObj(n, d) { //v3.0
  var p,i,x;  if(!d) d=document; if((p=n.indexOf("?"))>0&&parent.frames.length) {
    d=parent.frames[n.substring(p+1)].document; n=n.substring(0,p);}
  if(!(x=d[n])&&d.all) x=d.all[n]; for (i=0;!x&&i<d.forms.length;i++) x=d.forms[i][n];
  for(i=0;!x&&d.layers&&i<d.layers.length;i++) x=MM_findObj(n,d.layers[i].document); return x;
}

function MM_swapImage() { //v3.0
	var i,j=0,x,a=MM_swapImage.arguments; document.MM_sr=new Array; for(i=0;i<(a.length-2);i+=3)
	if ((x=MM_findObj(a[i]))!=null){document.MM_sr[j++]=x; if(!x.oSrc) x.oSrc=x.src; x.src=a[i+2];}
}

function drawValue(atrstring, val, drawon, drawoff) {
	var atrmax;
	eval("atrmax = " + atrstring + "max");
	for (var i = 1 ; i <= atrmax ; i++) {
	  var elem;
	  eval ("elem = document.getElementById(\"" + atrstring + i + "\")");
	  if (i <= val) {
		  elem.innerHTML=drawon;
	  } else {
		  elem.innerHTML=drawoff;
	  }
	  
	}
}

function setAttribute(atrstring, val, drawon, drawoff) {
   var oldatr;
   var newatr;
   eval ("oldatr = " + atrstring);
   if (val != oldatr) {
      eval (atrstring + " = val ");
      drawValue(atrstring, val, drawon, drawoff);
   } else {
      eval (atrstring + " = val - 1 ");
      drawValue(atrstring, val - 1, drawon, drawoff);
   }
   var elem;
   eval ("elem = document.getElementById(\"" + atrstring + "value\")");
   eval ("elem.innerHTML = atrstring + \" value: \" + " + atrstring);
}
//-->
</script>



<html>
<head>
<title>Ficha de Lobisomem: O Apocalipse</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf8">

<script type="text/javascript" src="typeface-0.11.js"></script>
<script type="text/javascript" src="werewolf_regular_basic.js"></script>
<script type="text/javascript" src="stucco_basic.js"></script>

<link href="ficha.css" rel="stylesheet" type="text/css" media="all" />

</head>

<body style="font-family: times, serif" bgcolor="#999999" link="#000000" vlink="#000000" alink="#000000" onLoad="MM_preloadImages('comics/archive/phd051109s.gif')">
<div align="center"><br>
  <table width="800" border="0" cellspacing="1" cellpadding="0" height="537" bgcolor="#666666">
    <tr bgcolor="#FFFFFF" valign="top" align="center"> 
      <td>
		<br /><br />
		<table width="720" border="0" cellspacing="0" cellpadding="0">
			<tr valign="top">
				<td align="center">
					<?php include("title.php"); ?>
					<br />
				</td>
			</tr>
		</table>
	  
        <table width="720" border="0" cellspacing="0" cellpadding="0">

					<tr valign="top"> 
						<td width="225">
							Nome:<br />
							Jogador:<br />
							Crônica:<br />
						</td>
						<td>&nbsp;</td>
						<td width="225"> 
							Raça:<br />
							Augúrio:<br />
							Tribo:<br />
						</td>
						<td>&nbsp;</td>
						<td width="225">
							Nome da Matilha:<br />
							Totem da Matilha:<br />
							Conceito:<br />
						</td>
					</tr>
				
        </table>

        <table width="720" border="0" cellspacing="0" cellpadding="0">
			<tr valign="top">
				<td>
					<?php $header = "Atributos"; include("sectionheader.php"); ?>
				</td>
			</tr>
			<tr valign="top"><td>
				<table width="720" border="0" cellspacing="0" cellpadding="0">
					<tr valign="top"> 
						<td width="225">
							<p align="center" class="typeface-js" style="font-family: Werewolf;line-height: 0.2"><font size=4>Fisicos</font></p>
							<?php $atrlist = array("Força", "Destreza", "Vigor");
								$varlist = array(strength, dexterity, stamina);
								$min=1; $max=5; include("atrlist.php");
							?>
						</td>
						<td>&nbsp;</td>
						<td width="225"> 
							<p align="center" class="typeface-js" style="font-family: Werewolf;line-height: 0.2"><font size=4>Sociais</font></p>
							<?php $atrlist = array("Carisma", "Manipulação", "Aparência");
								$varlist = array(charisma, manipulation, appearance);
								$min=1; $max=5; include("atrlist.php");
							?>
						</td>
						<td>&nbsp;</td>
						<td width="225">
							<p align="center" class="typeface-js" style="font-family: Werewolf;line-height: 0.2"><font size=4>Mentais</font></p>
							<?php $atrlist = array("Percepção", "Inteligência", "Raciocínio");
								$varlist = array(perception, intelligente, wits);
								$min=1; $max=5; include("atrlist.php");
							 ?>
						</td>
					</tr>
				</table>
			</td></tr>
        </table>
		
		<table width="720" border="0" cellspacing="0" cellpadding="0">
			<tr valign="top">
				<td>
					<?php $header = "Habilidades"; include("sectionheader.php"); ?>
				</td>
			</tr>
			<tr valign="top"><td>
				<table width="720" border="0" cellspacing="0" cellpadding="0">
					<tr valign="top"> 
						<td width="225">
							<p align="center" class="typeface-js" style="font-family: Werewolf;line-height: 0.2"><font size=4>Talentos</font></p>
							 <?php $atrlist = array("Prontidão", "Esportes", "Briga", "Esquiva", "Empatia", "Expressão", "Intimidação", "Instinto Primitivo", "Manha", "Lábia");
								$varlist = array(alertness, athletics, brawl, dodge, empathy, expression, intimidation, primalurge, streetwise, subterfuge);
								$min=0; $max=5; include("atrlist.php");
							 ?>
						</td>
						<td>&nbsp;</td>
						<td width="225">
							<p align="center" class="typeface-js" style="font-family: Werewolf;line-height: 0.2"><font size=4>Pericias</font></p>
							 <?php $atrlist = array("Emp. com animais", "Condução", "Protocolo", "Armas de Fogo", "Armas Brancas", "Liderança", "Atuação", "Ofícios", "Furtividade", "Sobrevivência");
								$varlist = array(animalken, drive, etiquette, firearms, melee, leadership, performance, repair, stealth, survival);
								$min=0; $max=5; include("atrlist.php");
							 ?>
						</td>
						<td>&nbsp;</td>
						<td width="225">
							<p align="center" class="typeface-js" style="font-family: Werewolf;line-height: 0.2"><font size=4>Conhecimentos</font></p>
							 <?php $atrlist = array("Computador", "Enigmas", "Investigação", "Direito", "Lingüística", "Medicina", "Ocultismo", "Política", "Rituais", "Ciência");
								$varlist = array(computer, enigmas, investigation, law, linguistics, medicin, occult, politics, rites, science);
								$min=0; $max=5; include("atrlist.php");
							 ?>
						</td>
					</tr>
				</table>
			</td></tr>
        </table>
		
		<table width="720" border="0" cellspacing="0" cellpadding="0">
			<tr valign="top">
				<td>
					<?php $header = "Vantagens"; include("sectionheader.php"); ?>
				</td>
			</tr>
			<tr valign="top"><td>
				<table width="720" border="0" cellspacing="0" cellpadding="0">
					<tr valign="top"> 
						<td width="225">
							<p align="center" class="typeface-js" style="font-family: Werewolf;line-height: 0.2"><font size=4>Antecedentes</font></p>
						</td>
						<td>&nbsp;</td>
						<td width="225"> 
							<p align="center" class="typeface-js" style="font-family: Werewolf;line-height: 0.2"><font size=4>Dons</font></p>
						</td>
						<td>&nbsp;</td>
						<td width="225">
							<p align="center" class="typeface-js" style="font-family: Werewolf;line-height: 0.2"><font size=4>Dons</font></p>
						</td>
					</tr>
				</table>
			</td></tr>
        </table>
		
		<img src="split.jpg" /><br />
		
		<table width="720" border="0" cellspacing="0" cellpadding="0">
			<tr valign="top"> 
				<td width="225">					
					<table width="225" border="0" cellspacing="0" cellpadding="0">
						<tr><td>
							<?php $header = "Renome"; include("smallheader.php"); ?>
						</td></tr>
						<tr><td><p align="center">
							<font class="typeface-js" style="font-family: Werewolf" size=3>Gloria</font>
							<?php $atr=glory; $min=0; $max=10; include("tenballs.php"); ?>
						</p></td></tr>
						<tr><td><p align="center">
							<font class="typeface-js" style="font-family: Werewolf" size=3>Honra</font>
							<?php $atr=honor; $min=0; $max=10; include("tenballs.php"); ?>
						</p></td></tr>
						<tr><td><p align="center">
							<font class="typeface-js" style="font-family: Werewolf" size=3>Sabedoria</font>
							<?php $atr=wisdom; $min=0; $max=10; include("tenballs.php"); ?>
						</p></td></tr>
						<tr><td style="background-image: url(splitsmall.jpg)">
							<p align="center" class="typeface-js" style="font-family: Werewolf"><font size=4 style="background-color: #FFFFFF; color: #FFFFFF">X<font style="color: #000000">Posto</font>X</font></p>
						</td></tr>
					</table>
				</td>
				<td>&nbsp;</td>
				<td width="225">
					<table width="225" border="0" cellspacing="0" cellpadding="0">
						<tr><td>
							<?php $header = "Furia"; include("smallheader.php"); ?>
							<?php $atr=rage; $min=0; $max=10; include("tenballs.php"); ?>
						</td></tr>
						<tr><td>
							<?php $header = "Gnose"; include("smallheader.php"); ?>
							<?php $atr=gnosis; $min=0; $max=10; include("tenballs.php"); ?>
						</td></tr>
						<tr><td>
							<?php $header = "Forca de Vontade"; include("smallheader.php"); ?>
							<?php $atr=willpower; $min=0; $max=10; include("tenballs.php"); ?>
						</td></tr>
					</table>
				</td>					
				<td>&nbsp;</td>
				<td width="225">
					<table width="225" border="0" cellspacing="0" cellpadding="0">
						<tr><td style="background-image: url(splitsmall.jpg)">
							<?php $header = "Vitalidade"; include("smallheader.php"); ?>
						</td></tr>
						<tr><td style="background-image: url(splitsmall.jpg)">
							<?php $header = "Fraqueza"; include("smallheader.php"); ?>
						</td></tr>
					</table>
				</td>
			</tr>
        </table>
		
        <table width="720" border="0" cellspacing="0" cellpadding="0">
          <tr>
            <td>
              <div align="center"><a onMouseOut="MM_swapImgRestore()" onClick="MM_swapImage('comicimage','','comics/archive/phd051109s.gif',1)"><img name=comicimage src="images/journal_fig.gif"></a></div>
            </td>
          </tr>
        </table>
        <table width="100%" border="0" cellspacing="0" cellpadding="15">
          <tr valign="bottom"> 
            <td> 
              <div align="left"> <font size="-1"><i>Continue stealth mode:</i> 
                <a href=http://www.phdcomics.com/archive_journal.php?n=1170>previous comic</a>
                - <a href=http://www.phdcomics.com/archive_journal.php?n=1172>next comic</a><br>
<i><a href=http://www.phdcomics.com/comics/archive.php?comicid=1171>The coast is clear</a></font> </div>
            </td>
            <td> 
              <div align="right"> <font size="-1"><i>Intl. Journal of <a href=javascript:popupwin('comics/tellafriend.php?comicid=1171','335','500','yes')>hey, tell a 
friend about this comic</i></a><br>
                  Vol. 4, No. 1171, pp.173, 2009		</font> </div>
            </td>
          </tr>
        </table>
      </td>
    </tr>
  </table>
</div>
<br>
</body>
</html>

