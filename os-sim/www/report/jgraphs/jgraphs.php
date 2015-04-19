<?php

$pathtoxml = dirname($_SERVER['REQUEST_URI']);

define("MAX_HOSTNAME_LEN", 30);
define("MAX_ALERTNAME_LEN", 30);

$proto = "http";
if (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] == "on")
    $proto = "https";

require_once("ossim_conf.inc");
$ossim_conf = $GLOBALS["CONF"];
$datapath = $ossim_conf->get_conf("ossim_link") . "/tmp/";
$base_dir = $ossim_conf->get_conf("base_dir");

$datapath = "$proto://$_SERVER[SERVER_ADDR]:$_SERVER[SERVER_PORT]/$datapath/";

function	jgraph_attack_graph($target, $hosts, $type = "Bar3D", $width = 450, $height = 250)
{
  global	$security_report;
  global	$datapath;
  global    $base_dir;

  if (!strcmp($target, "ip_src")) {
    if (!$fp = @fopen("$base_dir/tmp/ip_src.xml", "w")) {
        print "Error: <b>$datapath</b> directory must exists and be <br/>\n";
        print "writable by the user the webserver runs as";
        exit();
    }
  } else {
    if (!$fp = @fopen("$base_dir/tmp/ip_dst.xml", "w")) {
        print "Error: <b>$datapath</b> directory must exists and be <br/>\n";
        print "writable by the user the webserver runs as";
        exit();
    }
  }
  fwrite($fp, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n".
	 "<CategoryDataset>\n  <Series name=\"$target\">\n");
  $list = $security_report->AttackHost($target, $hosts);
  foreach ($list as $l) {
    $ip = $l[0];
    $occurrences = $l[1];
    $hostname = Host::ip2hostname($security_report->ossim_conn, $ip);
    $os_pixmap = Host_os::get_os_pixmap($security_report->ossim_conn, $ip);
    if(strlen($hostname) > MAX_HOSTNAME_LEN) $hostname = $ip;
    fwrite($fp, "    <Item>\n      <Key>$hostname</Key>\n      <Value>$occurrences</Value>\n    </Item>\n");
  }
  fwrite($fp, "  </Series>\n</CategoryDataset>\n\n");
  fclose ($fp);
  
  echo "
<applet archive=\"../java/jcommon-0.9.5.jar,../java/jfreechart-0.9.20.jar,../java/jossim-graph.jar\" code=\"net.ossim.graph.applet.OssimGraphApplet\" width=\"$width\" height=\"$height\" alt=\"You should see an applet, not this text.\">
    <param name=\"graphType\" value=\"$type\">";
  if (!strcmp($target, "ip_src"))
    echo "   <param name=\"xmlDataUrl\" value=\"$datapath/ip_src.xml\">";
  else
    echo "   <param name=\"xmlDataUrl\" value=\"$datapath/ip_dst.xml\">";
echo "
    <param name=\"alpha\" value=\"0.42f\">
    <param name=\"legend\" value=\"false\">
    <param name=\"tooltips\" value=\"false\">
    <param name=\"orientation\" value=\"HORIZONTAL\">
</applet>
";
}

function	jgraph_ports_graph($type = "Bar3D", $width = 400, $height = 250)
{
  global	$security_report;
  global	$datapath;
  global    $base_dir;
  
  if (!$fp = @fopen("$base_dir/tmp/ports.xml", "w")) {
        print "Error: <b>$datapath</b> directory must exists and be <br/>\n";
        print "writable by the user the webserver runs as";
        exit();
  }
  fwrite($fp, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n".
	 "<CategoryDataset>\n  <Series name=\"ports\">\n");
  $list = $security_report->Ports();
  foreach ($list as $l)
    {
      fwrite($fp, "    <Item>\n      <Key>$l[1] ($l[0])</Key>\n      <Value>$l[2]</Value>\n    </Item>\n");
    }
  fwrite($fp, "  </Series>\n</CategoryDataset>\n\n");
  fclose ($fp);
  
  echo "
<applet archive=\"../java/jcommon-0.9.5.jar,../java/jfreechart-0.9.20.jar,../java/jossim-graph.jar\" code=\"net.ossim.graph.applet.OssimGraphApplet\" width=\"$width\" height=\"$height\" alt=\"You should see an applet, not this text.\">
    <param name=\"graphType\" value=\"$type\">
    <param name=\"xmlDataUrl\" value=\"$datapath/ports.xml\">
    <param name=\"alpha\" value=\"0.42f\">
    <param name=\"legend\" value=\"false\">
    <param name=\"tooltips\" value=\"false\">
    <param name=\"orientation\" value=\"HORIZONTAL\">
</applet>
";
}

function	jgraph_nbevents_graph($type = "Pie3D", $width = 600, $height = 300)
{
  global	$security_report;
  global	$datapath;
  global    $base_dir;

  if (!$fp = @fopen("$base_dir/tmp/nbevents.xml", "w")) {
        print "Error: <b>$datapath</b> directory must exists and be <br/>\n";
        print "writable by the user the webserver runs as";
        exit();
  }
  fwrite($fp, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n".
	 "<PieDataset>\n");
  $list = $security_report->Events();  
  foreach ($list as $l)
    {
      if(strlen($l[0]) > MAX_ALERTNAME_LEN) $l[0] = substr($l[0],0,MAX_ALERTNAME_LEN) . "...";
      fwrite($fp, "  <Item>\n    <Key>$l[0]</Key>\n    <Value>$l[1]</Value>\n  </Item>\n");
    }
  fwrite($fp, "</PieDataset>\n");
  fclose ($fp);
  
  echo "
<applet archive=\"../java/jcommon-0.9.5.jar,../java/jfreechart-0.9.20.jar,../java/jossim-graph.jar\" code=\"net.ossim.graph.applet.OssimGraphApplet\" width=\"$width\" height=\"$height\" alt=\"You should see an applet, not this text.\">
    <param name=\"graphType\" value=\"$type\">
    <param name=\"xmlDataUrl\" value=\"$datapath/nbevents.xml\">
    <param name=\"alpha\" value=\"0.9f\">
    <param name=\"legend\" value=\"false\">
    <param name=\"tooltips\" value=\"false\">
    <param name=\"orientation\" value=\"HORIZONTAL\">
<!--    <param name=\"rotate\" value=\"true\"> -->
</applet>
";
}

function	jgraph_riskevents_graph($type = "Bar3D", $width = 400, $height = 250)
{
  global	$security_report;
  global	$datapath;
  global    $base_dir;
  
  if (!$fp = @fopen("$base_dir/tmp/riskevents.xml", "w")) {
        print "Error: <b>jgraphs</b> directory must be writable<br/>\n";
        print "by the user the webserver runs as";
        exit();
  }
  fwrite($fp, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n".
	 "<CategoryDataset>\n  <Series name=\"event by risk\">\n");
  $list = $security_report->EventsByRisk();
  foreach ($list as $l)
    {
      fwrite($fp, "    <Item>\n      <Key>$l[1] ($l[0])</Key>\n      <Value>$l[2]</Value>\n    </Item>\n");
    }
  fwrite($fp, "  </Series>\n</CategoryDataset>\n\n");
  fclose ($fp);
  
  echo "
<applet archive=\"../java/jcommon-0.9.5.jar,../java/jfreechart-0.9.20.jar,../java/jossim-graph.jar\" code=\"net.ossim.graph.applet.OssimGraphApplet\" width=\"$width\" height=\"$height\" alt=\"You should see an applet, not this text.\">
    <param name=\"graphType\" value=\"$type\">
    <param name=\"xmlDataUrl\" value=\"$datapath/riskevents.xml\">
    <param name=\"alpha\" value=\"0.42f\">
    <param name=\"legend\" value=\"false\">
    <param name=\"tooltips\" value=\"false\">
    <param name=\"orientation\" value=\"HORIZONTAL\">
</applet>
";
}

?>

