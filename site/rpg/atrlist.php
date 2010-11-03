<?php
   $lineheight="20px";

   echo("<table width=\"225\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">");
   echo("<tr>");
   echo("<td width=\"50%\">");
   echo("<p style=\"line-height: ${lineheight}; white-space: nowrap\">");

   for($i = 0, $size = sizeof($atrlist); $i < $size; ++$i)
   {
      echo ("$atrlist[$i] <br />");
   }

   echo("</p>");
   echo("</td>");
   echo("<td width=\"50%\">");
   echo("<p align=\"right\" style=\"line-height: ${lineheight}\">");

   for($i = 0, $size = sizeof($varlist); $i < $size; ++$i)
   {
      echo("<script type=\"text/javascript\">");
      echo("   var $varlist[$i] = $min;");
      echo("   var ${varlist[$i]}min = $min;");
      echo("   var ${varlist[$i]}max = $max;");
      echo("</script>");

      for ($j = 1; $j <= $min; $j++) {
      	echo("<font size=\"6\" style=\"font-family: arial\"><a id=\"${varlist[$i]}${j}\" href=\"javascript:setAttribute('${varlist[$i]}', $j, '●', '○')\">●</a></font>");
      }

      if ($max > $min) {
         for ($j = $min + 1; $j <= $max; $j++) {
            echo("<font size=\"6\" style=\"font-family: arial\"><a id=\"${varlist[$i]}${j}\" href=\"javascript:setAttribute('${varlist[$i]}', $j, '●', '○')\">○</a></font>");
         }
      }
   
      echo ("<br />");
   }
   echo("</p>");
   echo("</td>");
   echo("</tr>");
   echo("</table>");
?>
