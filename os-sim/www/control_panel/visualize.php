<?php
include ('classes/Locale.inc');
?>
<html>
<head>
  <title> <?php echo gettext("Control Panel"); ?> </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>

<body>

<?php
    if (!$backlog_id = $_GET["backlog_id"]) {
        echo gettext("Backlog ID required");
        exit();
    }

$proto = "http";
if ($_SERVER[HTTPS] == "on")
    $proto = "https";

require_once("ossim_conf.inc");
$ossim_conf = new ossim_conf();
$datapath = $ossim_conf->get_conf("ossim_link") . "/tmp/";
$javapath = $ossim_conf->get_conf("ossim_link") . "/java/";
$origpath = $ossim_conf->get_conf("ossim_link") . "/java/";
$base_dir = $ossim_conf->get_conf("base_dir");

$datapath = "$proto://$_SERVER[SERVER_ADDR]:$_SERVER[SERVER_PORT]/$datapath/$backlog_id.txt";
$imagepath = "$proto://$_SERVER[SERVER_ADDR]:$_SERVER[SERVER_PORT]/$javapath/images/";
$javapath = "$proto://$_SERVER[SERVER_ADDR]:$_SERVER[SERVER_PORT]/$javapath/";
?>
  <h1 align="center"> <?php echo gettext("Alarm viewer"); ?> </h1>

<applet archive="<?php echo $origpath; ?>/mm.mysql-2.0.14-bin.jar,<?php echo $origpath; ?>/scanmap3d.jar" code="net.ossim.scanmap.OssimScanMap3DApplet" width="400" height="400" alt="Applet de prueba">
        <param name="dataUrl" value="<?php echo $javapath; ?>/scanmap3d.conf">
        <param name="textFileDataUrl" value="<?php echo $datapath;?>">
        <param name="imagesBaseUrl" value="<?php echo $imagepath;?>">
</applet>


</body>
</html>


