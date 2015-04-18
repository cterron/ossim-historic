<?php

function getOptions($option, $line) 
{
    $pattern = "/$option:\s*([^;]+);/";
    
    if (preg_match_all($pattern, $line, $regs)) {
        return $regs[0];
    }
}

function printFirstOption($option, $line) 
{
    $pattern = "/$option:\s*([^;]+);/";
    
    if (preg_match($pattern, $line, $regs)) {
        return $regs[1];
    }
}

function parseOption($option) {

    $pattern = "/:\s*([^;]+);/";
    
    if (preg_match($pattern, $option, $regs)) {
        return $regs[1];
    }
}

function isSetOption($option, $line) {
    $pattern = "/$option;/";
    return preg_match($pattern, $line, $regs);
}

?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>OSSIM Framework</h1>
                                                                                
  <h2>Rule editor</h2>

<?php
    if (!$_GET["name"]) {
        echo "(Wrong argument)<br>\n";
        exit();
    }

    $rule_name = $_GET["name"];

    require_once ('ossim_conf.inc');
    require_once ('dir.php');
    require_once ('options.php');
                                                                                
    $ossim_conf = new ossim_conf();
    $snort_rules_path = $ossim_conf->get_conf("snort_rules_path");
?>

<h2><u><?php echo $rule_name; ?></u></h2>

<table border="1" align="center" width="100%">
  <tr>
    <th><font size="-2">Name</font></th>
    <th><font size="-2">Action</font></th>
    <th><font size="-2">Protocol</font></th>
    <th><font size="-2">SRC IP</font></th>
    <th><font size="-2">SRC Ports</font></th>
    <th><font size="-2">Dir</font></th>
    <th><font size="-2">DEST IP</font></th>
    <th><font size="-2">DEST Ports</font></th>
    <th><font size="-2">Content</font></th>
    <th><font size="-2">Options</font></th>
  </tr>

<?php
if (!$fd = fopen ($snort_rules_path . $rule_name, "r")) {
    echo "Error opening file\n";
    exit;
}
$nline = 0;
while (!feof($fd)) 
{
    $line = fgets($fd, 4096);    
    $nline++;

/*
    Rule action (alert, log, pass, activate, dynamic)
    Protocol (tcp, udp, icmp, ip)
    SRC IP Address (any, xxx.xxx.xxx.xxx/mask, $VARIABLE)
    SRC Ports (any, single_port, port:port, !negated_port, $VARIABLE)
    Direction (->, <>)
    DEST IP Address (any, xxx.xxx.xxx.xxx/mask, $VARIABLE)
    DEST Ports (any, single_port, port:port, !negated_port. $VARIABLE)
    
 */

    if (preg_match
        ("/^(alert|log|pass|activate|dynamic)". /* rule action     */
         "\s*(tcp|udp|icmp|ip)" .               /* protocol        */
         "\s*([^\s]+)\s?([^\s]+)" .             /* SRC IP & Ports  */
         "\s*(->|<>)" .                         /* direction       */
         "\s*([^\s]+)\s?([^\s]+)/",             /* SRC IP & Ports  */
         $line, 
         $regs)) 
    {
?>
    <tr>
      <td align="center"><?php echo printFirstOption("msg", $line); ?></td>
      <td align="center"><?php echo $regs[1]?></td>
      <td align="center"><?php echo $regs[2]?></td>
      <td align="center"><?php echo $regs[3]?></td>
      <td align="center"><?php echo $regs[4]?></td>
      <td align="center"><?php echo $regs[5]?></td>
      <td align="center"><?php echo $regs[6]?></td>
      <td align="center"><?php echo $regs[7]?></td>
      <td align="center">
        <?php 
            $options = getOptions("content", $line); 
            $count = count($options);
            for ($i = 0; $i < $count; $i++) {
                if ($i > 0)  echo "<br>";
                echo parseOption($options[$i]);
            }
        ?>
      </td>
      <td align="left">
        <table width="100%">



<?php 
    /* opciones con argumentos */
    foreach ($ruleOptions as $roption) {
        if ($options = getOptions($roption, $line)) 
        {
            $count = count($options);
            for ($i = 0; $i < $count; $i++) {
?>
          <tr><td align="left">
<?php
                echo "<b>$roption</b>: " . parseOption($options[$i] . "\n");
            }
?>
          </td></tr>
<?php
        }
    }

    /* opciones sin argumentos */
    foreach ($ruleSingleOptions as $soption) {
        if (isSetOption($soption, $line)) {
?>          <tr><td align="left">
<?php
            echo "<b>$soption</b>\n";
?>          </td></tr>
<?php
        }
    }
?>
        </table>
      </td>
    </tr>
<?php
    }

}
fclose($fd);
?>

</table>

</body>
</html>

