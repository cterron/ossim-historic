<html>
<head>
  <title>OSSIM</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="style/style.css"/>
</head>
<body>


<?php
    require_once ('ossim_conf.inc');
    $conf = new ossim_conf();
?>

  <table align="center" border="0">
    <tr>
     <th align="center">
        [<a href="host/host.php" title="host management" 
           target="main">Hosts</a>]
        [<a href="scan/scan.php" title="host scanning" 
           target="main">Scan</a>]
        [<a href="rrd_conf/rrd_conf.php" title="RRD Conf Management" 
           target="main">RRD Config</a>]
        [<a href="net/net.php" title="port management" 
           target="main">Networks</a>]
        [<a href="port/port.php" title="port management" 
           target="main">Ports</a>]
        [<a href="sensor/sensor.php" title="sensor management" 
           target="main">Sensors</a>]
        [<a href="signature/signature.php" title="sensor management" 
           target="main">Signatures</a>]
        [<a href="editor/editor.php" title="rule editor" 
           target="main">Rule Editor</a>]
        [<a href="policy/policy.php" title="policy management" 
           target="main">Policy</a>]<br/>
        [<a href="control_panel/index.php" title="OSSIM Control Panel" 
           target="main"><font color="#991e1e">Control Panel</font></a>]
        [<a href="riskmeter/index.php" title="OSSIM riskmeter" 
           target="main"><font color="#991e1e">Riskmeter</font></a>]
        [<a href="graphs/index.php" title="OSSIM graph viewer" 
           target="main"><font color="#991e1e">Graphs</font></a>]
        [<a href="<?php echo $conf->get_conf("acid_link"); ?>" 
           title="(SNORT)" 
           target="main"><font color="#991e1e">Forensics</font></a>]
        [<a href="<?php echo $conf->get_conf("ntop_link"); ?>/NetNetstat.html" 
           title="(NTOP - Active TCP Sessions)" 
           target="main"><font color="#991e1e">Session Monitor</font></a>]
        [<a href="<?php echo $conf->get_conf("ntop_link"); ?>" 
           title="(NTOP)" 
           target="main"><font color="#991e1e">Usage Monitor</font></a>]
        [<a href="<?php echo $conf->get_conf("opennms_link"); ?>" 
           title="(OpenNMS)" 
           target="main"><font color="#991e1e">Service availability</font></a>]
        [<a href="wizard/wizard.php" title="configuration wizard" 
           target="main"><font color="black">Wizard</a></a>]
        [<a href="conf/modifyconfform.php" title="framework config" 
           target="main"><font color="black">Conf</a></a>]
      </th>
    </tr>
  </table>

</body>
</html>
