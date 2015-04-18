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

<?php
    if ($_GET["menu"] == "main") {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=main" 
           title="Main"><font color="#991e1e">Main</font></a>]
<?php
    } else {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=main" 
           title="Main">Main</a>]
    
<?php
    } 
    
    if ($_GET["menu"] == "inventory") {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=inventory" 
           title="Inventory"><font color="#991e1e">Inventory</font></a>]
<?php
    } else {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=inventory" 
           title="Inventory">Inventory</a>]
<?php
    }
    if ($_GET["menu"] == "reports") {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=reports" 
           title="Reports"><font color="#991e1e">Reports</font></a>]
<?php
    } else {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=reports" 
           title="Reports">Reports</a>]
<?php
    }
?>


<?php
    if ($_GET["menu"] == "config") {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=config" 
           title="Configuration"><font color="#991e1e">Configuration</font></a>]
<?php
    } else {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=config" 
           title="Configuration">Configuration</a>]
<?php
    }
?>


<?php
    if ($_GET["menu"] == "tools") {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=tools" 
           title="Tools"><font color="#991e1e">Tools</font></a>]
<?php
    } else {
?>
        [<a href="<?php echo $_SERVER["PHP_SELF"]?>?menu=tools" 
           title="Tools">Tools</a>]
<?php
    }
?>



<!--
    submenu 
-->

<?php
    if ($_GET["menu"] == "main") {
?>
        <br/>
        [<a href="control_panel/index.php" 
           title="OSSIM Control Panel"
           target="main">Control Panel</a>]
        [<a href="policy/policy.php" title="policy management" 
           target="main">Policy</a>]
        [<a href="riskmeter/index.php" title="OSSIM riskmeter" 
           target="main">Riskmeter</a>]
        [<a href="vulnmeter/index.php" title="OSSIM vulnmeter" 
           target="main">Audit</a>]
        [<a href="<?php 
           echo $conf->get_conf("ntop_link"); ?>"
           title="(NTOP)" 
           target="main">Usage Monitor</a>]

<?php
    } elseif ($_GET["menu"] == "inventory") {
?>
        <br/>
        [<a href="host/host.php" title="host management" 
           target="main">Hosts</a>]
        [<a href="net/net.php" title="port management" 
           target="main">Networks</a>]
        [<a href="port/port.php" title="port management" 
           target="main">Ports</a>]
 
<?php
    } elseif ($_GET["menu"] == "reports") {
?>
        <br/>
        [<a href="<?php 
           echo $conf->get_conf("ntop_link"); ?>/sortDataProtos.html"
           title="(NTOP)" 
           target="main">Usage Monitor</a>]
        [<a href="<?php 
           echo $conf->get_conf("ntop_link"); ?>/NetNetstat.html"
           title="(NTOP - Active TCP Sessions)" 
           target="main">Session Monitor</a>]
        [<a href="<?php 
           echo $conf->get_conf("acid_link"); ?>" 
           title="(SNORT)" 
           target="main">Forensics</a>]

<?php
    } elseif ($_GET["menu"] == "config") {
?>
        <br/>
        [<a href="sensor/sensor.php" title="sensor management" 
           target="main">Sensors</a>]
        [<a href="signature/signature.php" title="sensor management" 
           target="main">Signatures</a>]
        [<a href="editor/editor.php" title="rule editor" 
           target="main">Rule Editor</a>]
        [<a href="rrd_conf/rrd_conf.php" title="RRD Conf Management" 
           target="main">RRD Config</a>]
        [<a href="directives/index.php" title="directive editor" 
           target="main">Directive Editor</a>]
        [<a href="conf/modifyconfform.php" title="Framework Configuration" 
           target="main">Framework</a>]
<?php
    } elseif ($_GET["menu"] == "tools") {
?>
        <br/>
        [<a href="scan/scan.php" title="host scanning" 
           target="main">Scan</a>]
<?php
    }
?>

      </th>
    </tr>
  </table>

</body>
</html>
