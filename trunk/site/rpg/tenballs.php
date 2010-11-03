<?php
	$lineheight="20px";   
	echo("<p align=\"center\" style=\"line-height: ${lineheight}\">");
	echo("<script type=\"text/javascript\">");
	echo("var $atr = $min;");
	echo("var ${atr}min = $min;");
	echo("var ${atr}max = $max;");
	echo("var ${atr}cur = $min;");
	echo("var ${atr}curmin = $min;");
	echo("var ${atr}curmax = $max;");
	echo("</script>");

	for ($j = 1; $j <= $min; $j++) {
		echo("<font size=\"6\"><a id=\"${atr}${j}\" style=\"font-family: arial\" href=\"javascript:setAttribute('${atr}', $j, '●', '○')\">●</a></font>");
	}

	if ($max > $min) {
		for ($j = $min + 1; $j <= $max; $j++) {
			echo("<font size=\"6\"><a id=\"${atr}${j}\" style=\"font-family: arial\" href=\"javascript:setAttribute('${atr}', $j, '●', '○')\">○</a></font>");
		}
	}
	
	echo ("<br />");
	
	for ($j = 1; $j <= $min; $j++) {
		echo("<font size=\"6\" style=\"font-family: arial\"><a id=\"${atr}cur${j}\" href=\"javascript:setAttribute('${atr}cur', $j, '■', '□')\">■</a></font>");
	}

	if ($max > $min) {
		for ($j = $min + 1; $j <= $max; $j++) {
			echo("<font size=\"6\" style=\"font-family: arial\"><a id=\"${atr}cur${j}\" href=\"javascript:setAttribute('${atr}cur', $j, '■', '□')\">□</a></font>");
		}
	}

	echo ("<br /><br />");

	echo("</p>");
?>
