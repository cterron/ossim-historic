<?php
require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
require_once 'classes/Member_status.inc';

Session::logcheck("MenuConfiguration", "ConfigurationMaps");

$map_id = GET('map_id');
$db = new ossim_db();
$conn = $db->connect();
$status = new Member_status;
?>
<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
  <style>
  td {
      border-width: 0px;
  }
  img {
      border-width: 0px;
  }
  </style>
</head>
<body>
<h3><?=_("Select item type")?></h3>

<table align="center" width="60%">
<tr>
    <td><a href="./positions_map.php?map_id=<?=$map_id?>&type=sensor"><img src="<?=$status->get_icon('sensor', 'ok')?>"></a></td>
    <td><a href="javascript: alert('<?=_("Not implemented yet")?>')"><img src="../js/OpenLayers/img/marker-green.png"></a></td>
    <td><a href="javascript: alert('<?=_("Not implemented yet")?>')"><img src="../js/OpenLayers/img/marker-green.png"></a></td>
</tr>
    <td><a href="./positions_map.php?map_id=<?=$map_id?>&type=sensor"><?=_("Sensors")?></a></td>
    <td><a href="javascript: alert('<?=_("Not implemented yet")?>')"><?=_("Hosts")?></a></td>
    <td><a href="javascript: alert('<?=_("Not implemented yet")?>')"><?=_("Networks")?></a></td>
</td>
</table>