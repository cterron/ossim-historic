<?php
require_once 'classes/Session.inc';
Session::logcheck("MenuConfiguration", "ConfigurationMaps");

$db = new ossim_db();
$conn = $db->connect();

if (GET('delete')) {
    $id = GET('delete');
    $sql = "DELETE FROM map_element WHERE map_id = ?";
    if (!$conn->Execute($sql, array($id))) {
        die($conn->ErrorMsg());
    }
    $sql = "DELETE FROM map WHERE id = ?";
    if (!$conn->Execute($sql, array($id))) {
        die($conn->ErrorMsg());
    }
}

$sql = "SELECT
            m.id, m.name, m.engine, count(e.id) as num
        FROM
            map m
        LEFT JOIN map_element AS e ON m.id = e.map_id 
        GROUP BY m.id";
$rows = $conn->GetArray($sql);
if ($rows === false) {
    die(ossim_error($conn->ErrorMsg()));
}
?>
<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
<div style="width=100%; text-align: right">[<a href="./new.php"><?=_("New map")?></a>]</div>
<h3><?=_("Configured maps")?></h3>
<? if (!count($rows)) { ?>
    <center><i><?=_("No configured map found, please configure one using the 'New map' option")?></i></center>
<? } else { ?>
<table align="center" width="80%">
    <tr>
        <th><?=_("Map name")?></th>
        <th><?=_("Map type")?></th>
        <th><?=_("#pos")?></th>
        <th><?=_("Actions")?></th>
    </tr>
    <? foreach ($rows as $r) { ?>
    <tr>
        <td><?=$r['name']?></td>
        <td><?=$r['engine']?></td>
        <td><?=$r['num']?></td>
        <td nowrap>
        [<a href="./positions.php?map_id=<?=$r['id']?>"><?=_("set positions")?></a>]&nbsp;
        [<a href="./draw_openlayers.php?map_id=<?=$r['id']?>"><?=_("view map")?></a>]&nbsp;
        [<a href="<?=$_SERVER["PHP_SELF"]?>?delete=<?=$r['id']?>"><?=_("delete")?></a>]</td>
    </tr>
    <? } ?>
</table>
<? } ?>
</body></html>