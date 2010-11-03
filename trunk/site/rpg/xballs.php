<?php
   echo("<script type=\"text/javascript\">");
   echo("   var $atr = $min;");
   echo("   var ${atr}min = $min;");
   echo("   var ${atr}max = $max;");
   echo("</script>");

   $lineheight = 0.9;

   $i = 1;
   for ($i = 1; $i <= $min; $i++) {
   	echo("<font size=\"5\"><a id=\"${atr}${i}\" href=\"javascript:setAttribute('$atr', $i)\">●</a></font>");
   }

   if ($max > $min) {
      for ($i = $min + 1; $i <= $max; $i++) {
         echo("<font size=\"5\"><a id=\"${atr}${i}\" href=\"javascript:setAttribute('$atr', $i)\">○</a></font>");
      }
   }
?>

<br />
