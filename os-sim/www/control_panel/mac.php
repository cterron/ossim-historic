<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuControlPanel", "ControlPanelAnomalies");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("OSSIM Framework - Mac list"); ?> </h1>

<?php
require_once 'ossim_db.inc';
require_once 'classes/Host_mac.inc';
require_once 'classes/Host.inc';
require_once 'classes/Util.inc';
require_once 'classes/Security.inc';
?>

<?php

$ROWS = 50;


$inf = GET('inf');
$sup = GET('sup');
$show_anom = GET('show_anom');
$ex_mac  = GET('ex_mac');
$ex_macs = GET('ex_macs');
$num = GET('num');

ossim_valid($inf, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("inf"));
ossim_valid($sup, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("order"));
ossim_valid($show_anom, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("show_anom"));
ossim_valid($ex_mac, OSS_IP_ADDR, OSS_NULLABLE, 'illegal:'._("ex_mac"));
ossim_valid($ex_macs, OSS_IP_ADDR, OSS_NULLABLE, 'illegal:'._("ex_macs"));
ossim_valid($num, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("num"));


if (ossim_error()) {
        die(ossim_error());
}

if (empty($num))
    $num = $ROWS;
if (empty($inf))
    $inf = 0;
if ((empty($sup)) && ($num != "all"))
    $sup = $inf + $num;

?>            

<?php
$db = new ossim_db();
$conn = $db->connect();
if ($show_anom != "1") {
    $count = Host_mac::get_list_count($conn);
    if ($num == "all") {
        $sup = $count;
        $inf = 0;
    }
    $host_mac_list = Host_mac::get_list($conn, $inf, $sup);
} else {
    $host_mac_list = Host_mac::get_anom_list($conn, "all");
    $count = count($host_mac_list);
    $sup = $count;
    $inf = 0;
}
?>

<?php if ($show_anom != "1") { ?>
<table align="center">
<?php echo gettext("Show");?>
<form method="GET" action="mac.php">
<input type="hidden" name="inf" value="<?php echo $inf ?>"/>
<select name="num" onChange="submit()">
<option value="10"  <?if ($num == "10") echo "SELECTED"; ?>>10</option>
<option value="50"  <?if ($num == "50") echo "SELECTED"; ?>>50</option>
<option value="100" <?if ($num == "100") echo "SELECTED"; ?>>100</option>
<option value="all" <?if ($num == "all") echo "SELECTED"; ?>>All</option>
</select>
<?php echo gettext(" per page"); ?>
</table>
</br>
<?php } ?>

<?php 
if ($show_anom) 
    echo "<a href=\"mac.php\">".gettext("Showing only anomalies, click here to see the complete mac list")."</a>";
else 
    echo "<a href=\"mac.php?show_anom=1\">".gettext("Click here to see the only the anomalies")."</a>";
 ?>
<table width="100%">
<?php if ($num != "all"){ ?>
    <tr>
       <td colspan="12"> 
<?php
    $inf_link = $_SERVER["PHP_SELF"] .
            "?sup=" . ($sup - $num) .
            "&inf=" . ($inf - $num) .
            "&num=" . $num;
    $sup_link = $_SERVER["PHP_SELF"] .
        "?sup=" . ($sup + $num) .
        "&inf=" . ($inf + $num) .
        "&num=" . $num; 
    
    $first_link = $_SERVER["PHP_SELF"] .
        "?sup=" .  $num .
        "&inf=" . "0" .
        "&num=" . $num; 
    
    $last_link = $_SERVER["PHP_SELF"] .
        "?sup=" . $count .
        "&inf=" . ($count - $num) .
        "&num=" . $num; 
    ?>
    <table width="100%" bgcolor="#EFEFEF">       
    <td align=left>
    <?php
    if ($inf != "0"){ 
        echo "<a href=\"$first_link\">";  printf(gettext("First")); echo "</a>";
    }
    ?>
    </td>
    <td align="center">
    <?php
    if ($inf >= $num) {
        echo "<a href=\"$inf_link\">&lt;-"; printf(gettext("Prev %d"),$num); echo "</a>";
    }
    ?>
    <?php
    if ($sup < $count) {
        echo "&nbsp;&nbsp;("; printf(gettext("%d-%d of %d"),$inf, $sup, $count); echo ")&nbsp;&nbsp;";
        echo "<a href=\"$sup_link\">"; printf(gettext("Next %d"), $num); echo " -&gt;</a>";
    } else {
        echo "&nbsp;&nbsp;("; printf(gettext("%d-%d of %d"),$inf, $count, $count); echo ")&nbsp;&nbsp;";
    }
    ?>
    </td>
    <td align="right">
    <?php
    if ($sup < $count) {
        echo "<a href=\"$last_link\">"; printf(gettext("Last")); echo "</a>";
    }
    ?>
    </td>
  
    </table>
      </tr>

      <tr>

<?php } ?>

<tr>
<td align="center" colspan="12">
<input type="submit" value=" <?php echo gettext("OK"); ?> ">
<input type="reset" value=" <?php echo gettext("reset"); ?> "> </td>
</tr>
<tr>
<th><?php echo "#"; ?></th>
<th><?php echo "Host"; ?></th>
<th><?php echo gettext("Sensor [interface]"); ?> </th>
<th><?php echo "Mac"; ?></th>
<th><?php echo gettext("Vendor"); ?> </th>
<th><?php echo "Date"; ?></th>
<th><?php echo gettext("Previous Mac"); ?> </th>
<th><?php echo gettext("Previous Vendor"); ?> </th>
<th><?php echo gettext("Previous Date"); ?> </th>
<th><?php echo gettext("Delta"); ?> </th>
<th><?php echo gettext("Ack"); ?> </th>
<th><?php echo gettext("Ignore"); ?> </th>
</tr>

<form action="handle_mac.php" method="GET">


<?php 
if ($host_mac_list) {
     $row = 0;
     $aux = 0;
    foreach($host_mac_list as $host_mac) {
?>

<tr <?php  if ($host_mac["mac"] != $host_mac["old_mac"]) echo 'bgcolor="#f7a099"';
		else echo 'bgcolor="#bbcadd"';
?>>
<?php
        $delta = Util::date_diff($host_mac["date"], $host_mac["old_date"], 'yMdhms');
        if ($delta == "00:00:00") $delta = "-";
?>
<td>
<?php
if (($ex_mac == $host_mac["ip"]) && ($ex_macs == $host_mac["sensor"])) {
?>
<a href="<?php echo $_SERVER["PHP_SELF"]."?sup=".$sup."&inf=".$inf."&num=".$num;
if ($show_anom == "1") 
    echo "&show_anom=1" ?>"><img src="../pixmaps/arrow.gif" border=\"0\"></e>
<?php } else { ?>
<a href="<?php echo
$_SERVER["PHP_SELF"]."?inf=".$inf."&sup=".$sup."&num=".$num."&ex_mac=".$host_mac["ip"]."&ex_macs=".$host_mac["sensor"];
if ($show_anom == "1") echo "&show_anom=1"; ?>"><img src="../pixmaps/arrow2.gif" border=\"0\"></e>
<?php
}

?>
</td>
<td><?php echo $host_mac["ip"];?></td>
<td><?php echo $host_mac["sensor"]."[".$host_mac["interface"]."]";?></td>
<td><?php echo $host_mac["mac"];?></td>
<td><?php echo htm($host_mac["vendor"]);?>&nbsp;</td>
<td><?php echo $host_mac["date"];?></td>
<td><?php echo $host_mac["old_mac"];?></td>
<td><?php echo htm($host_mac["old_vendor"]);?>&nbsp;</td>
<td><?php echo $host_mac["old_date"]?></td>
<td><?php echo $delta; ?></td>
<td>
<input type="checkbox" name="ip,<?php echo $host_mac["ip"];?>,<?php echo $host_mac["sensor"];?>,<?php
echo $host_mac["date"];?>" value="<?php echo "ack".$host_mac["ip"];?>" <? if ($host_mac["mac"] == $host_mac["old_mac"]) echo "disabled" ?> ></input>
</td>
<td>
<input type="checkbox" name="ip,<?php echo $host_mac["ip"];?>,<?php echo $host_mac["sensor"];?>,<?php
echo $host_mac["old_date"];?>" value="<?php echo "ignore".$host_mac["ip"];?>" <? if ($host_mac["mac"] == $host_mac["old_mac"]) echo "disabled" ?> ></input>
</td>
</tr>
<?php 
if (($ex_mac == $host_mac["ip"]) && ($ex_macs == $host_mac["sensor"])) {


if ($host_mac_ip_list = Host_mac::get_ip_list($conn, $host_mac["ip"],$host_mac["sensor"])) {

	foreach ($host_mac_ip_list as $host_mac_ip){
 		 $delta = Util::date_diff($host_mac_ip["date"], $host_mac_ip["old_date"], 'yMdhms');
        	 if ($delta == "00:00:00") $delta = "-";
	  ?>
	  <tr bgcolor="#eac3c3">
	  <td>&nbsp;</td>
	  <td><?php echo $host_mac_ip["ip"];?></td>
	  <td><?php echo $host_mac_ip["sensor"]."[".$host_mac_ip["interface"]."]";?></td>
	  <td><?php echo $host_mac_ip["mac"];?></td>
	  <td><?php echo htm($host_mac_ip["vendor"]);?>&nbsp;</td>
	  <td><?php echo $host_mac_ip["date"];?></td>
	  <td><?php echo $host_mac_ip["old_mac"];?></td>
	  <td><?php echo htm($host_mac_ip["old_vendor"]);?>&nbsp;</td>
	  <td><?php echo $host_mac_ip["old_date"]?></td>
	  <td <?php if ($host_mac_ip["mac"] != $host_mac_ip["old_mac"]) echo
'bgcolor="#f7a099"';?>><?php echo $delta; ?>
      </td>
<td>
<input type="checkbox" name="ip,<?php echo $host_mac["ip"];?>,<?php echo $host_mac["sensor"];?>,<?php
echo $host_mac["date"];?>" value="<?php echo "ack".$host_mac["ip"];?>" <? if ($host_mac["mac"] == $host_mac["old_mac"]) echo "disabled" ?> ></input>
</td>
<td>
<input type="checkbox" name="ip,<?php echo $host_mac["ip"];?>,<?php echo $host_mac["sensor"];?>,<?php
echo $host_mac["old_date"];?>" value="<?php echo "ignore".$host_mac["ip"];?>" <? if ($host_mac["mac"] == $host_mac["old_mac"]) echo "disabled" ?> ></input>
</td>
</tr>	  
<?php
	}

}
        }
    }
}
    $db->close($conn);
?>
<tr>
<td align="center" colspan="12">
<input type="submit" value=" <?php echo gettext("OK"); ?> ">
<input type="reset" value=" <?php echo gettext("reset"); ?> "></td>
</tr>

</form>
<?php if ($num != "all"){ ?>
     <tr>
        <td colspan="12">
<?php

    $inf_link = $_SERVER["PHP_SELF"] .
            "?sup=" . ($sup - $num) .
            "&inf=" . ($inf - $num) .
            "&num=" . $num;
    $sup_link = $_SERVER["PHP_SELF"] .
        "?sup=" . ($sup + $num) .
        "&inf=" . ($inf + $num) .
        "&num=" . $num;
    $first_link = $_SERVER["PHP_SELF"] .
        "?sup=" .  $num .
        "&inf=" . $inf .
        "&num=" . $num;
    $last_link = $_SERVER["PHP_SELF"] .
        "?sup=" . $count .
        "&inf=" . ($count - $num) .
        "&num=" . $num;
?>

    <table width="100%" bgcolor="#EFEFEF">
    <td align=left>
    <?php
    if ($inf != "0"){
        echo "<a href=\"$first_link\">";  printf(gettext("First")); echo "</a>";
    }
    ?>
    </td>
    <td align="center">
    <?php
    if ($inf >= $num) {
        echo "<a href=\"$inf_link\">&lt;-"; printf(gettext("Prev %d"),$num); echo "</a>";
    }
    ?>
    <?php
    if ($sup < $count) {
        echo "&nbsp;&nbsp;("; printf(gettext("%d-%d of %d"),$inf, $sup, $count); echo ")&nbsp;&nbsp;";
        echo "<a href=\"$sup_link\">"; printf(gettext("Next %d"), $num); echo " -&gt;</a>";
    } else {
        echo "&nbsp;&nbsp;("; printf(gettext("%d-%d of %d"),$inf, $count, $count); echo ")&nbsp;&nbsp;";
    }
    ?>
    </td>
    <td align="right">
    <?php
    if ($sup < $count) {
        echo "<a href=\"$last_link\">"; printf(gettext("Last")); echo "</a>";
    }
    ?>
    </td>

    </table>

        </td>
      </tr>

      <tr>

<?php } ?>


</table>
</body>
</html>

