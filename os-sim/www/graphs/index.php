<html>
<head>
  <title></title>
  <meta http-equiv="refresh" content="15">
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
<SCRIPT>
function load(id){
window.open("./VGJ-applet/index.php?graph_id="+id,"Graph","heigth=640,width=660,alwayRaised=yes");
}
function load2(){
window.open("./VGJ/index.html","Graph");
}
</SCRIPT>
</head>
<body>

  <h1 align="center">OSSIM Framework</h1>
  <h2 align="center">Graphs</h2>

<?php

require_once ('ossim_db.inc');
require_once ('classes/Graph.inc');
require_once ('classes/Host.inc');
require_once ('classes/Link.inc');
require "func.php";


$db = new ossim_db();
$conn = $db->connect();

if ($_GET["graph_id"]) {
    $id_list[] = mysql_escape_string($_GET["graph_id"]);
} else {
    $id_list = Graph::get_id_list($conn, $graph_id);
    if (count($id_list) == 0) exit();

}

?>

<iframe src="generate_main.php" scrolling="no" height=0 width=0 frameborder=0>
</iframe>
<CENTER>
<A HREF="javascript:cargar2()"> Show Graph </A>
</CENTER>
<table align="center" border="1">
<?php
if($id_list)
    foreach ($id_list as $id) {
?>
<iframe src="generate.php?id=<?php echo $id?>" scrolling="no" height=0 width=0 frameborder=0>
</iframe>

  <tr><th colspan="2" align="center">Graph <?php echo $id ?></th></tr>
  <tr>
<!--
  <th>Hosts</th>
-->
  <th>Links</th></tr>
  <tr>

<!--
    <td align="center">
      <table align="center" width="100%">
-->
<?php
/*
    settype($id, "int");    
    $graph_list = Graph::get_list($conn, "WHERE id = $id");
    if($graph_list)
    foreach ($graph_list as $graph) {
        $ip = $graph->get_ip();
        $hostname = Host::ip2hostname($conn, $ip);       
*/
?>
<!--
      <tr><td align="center"><?php // echo $hostname ?></td></tr>
-->
<?php
/*
    }
*/
?>
<!--
      </table>
    </td>
-->
    <td>
      <table align="center" width="100%">
<?php

    $link_list = Graph::get_link_list($conn, $id);
    if ($link_list)  {
        foreach ($link_list as $link) {
    
            $source_ip   = $link->get_source();
            $dest_ip     = $link->get_dest();
            $occurrences = $link->get_occurrences();
       
            $source_hostname = Host::ip2hostname($conn, $source_ip);
            $dest_hostname   = Host::ip2hostname($conn, $dest_ip);
?>
      <tr>
        <td align="center"><?php echo $source_hostname ?></td>
        <td align="center">-&gt;</td>
        <td align="center"><?php echo $dest_hostname ?></td>
        <td align="center"><?php echo $occurrences ?></td>
      </tr>
<?php
        }
    }
?>
      </table>
    </td>
  </tr>
  <tr><td colspan="2"></td></tr>
<?php
}
?>
</table>
</body>

<?php 
    $db->close($conn); 
?>
