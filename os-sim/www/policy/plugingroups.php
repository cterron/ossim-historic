<?php
require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
require_once 'classes/Plugingroup.inc';
require_once 'ossim_db.inc';

Session::logcheck("MenuPolicy", "PolicyPluginGroups");

$db = new ossim_db();
$conn = $db->connect();

$groups = Plugingroup::get_list($conn);
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
  <script src="../js/prototype.js" type="text/javascript"></script>
</head>
<body>

  <h1> <?= _("Plugin Groups") ?> </h1>
<script>
function toggle_info(id)
{
    Element.toggle('plugins'+id);
    var img = 'img'+id;
    if ($(img).src.match(/arrow2/)) {
        $(img).src = '../pixmaps/arrow.gif';
    } else {
        $(img).src = '../pixmaps/arrow2.gif';
    }
}
    
</script>

<p style="text-align: right">
    <a href="modifyplugingroupsform.php?action=new"><?=_("Insert new group")?></a>
</p>

<table width="95%" align="center">
    <tr>
        <th><?=_("ID")?></th>
        <th><?=_("Name")?></th>
        <th><?=_("Description")?></th>
        <th><?=_("Actions")?></th>
    </tr>
    <? foreach ($groups as $group) {
        $id = $group->get_id();
    ?>
        <tr>
            <td NOWRAP>
                <img id="img<?=$id?>" src="../pixmaps/arrow2.gif" border="none"
                     onClick="javascript: toggle_info('<?=$id?>');"
                > <b><?=$id?></b>
            </td>
            <td NOWRAP><b><?=htm($group->get_name())?></b></td>
            <td width="70%" style="text-align: left"><?=htm($group->get_description())?></td>
            <td width="1%" NOWRAP>
                <a href="modifyplugingroupsform.php?action=edit&id=<?=$id?>"><?=_("Edit")?></a> -
                <a href="modifyplugingroups.php?action=delete&id=<?=$id?>"><?=_("Delete")?></a>
            </td>
        </tr>
        <tr id="plugins<?=$id?>" style="display: none;">
            <td>&nbsp;</td>
            <td>&nbsp;</td>
            <td width="100%" style="text-align: left;" NOWRAP>
                <table width="50%" align="left" style="border-width: 0px">
                <? foreach ($group->get_plugins() as $p) { ?>
                    <tr>
                        <td><?=$p['id']?></td>
                        <td><?=$p['name']?></td>
                        <td NOWRAP><?=$p['descr']?></td>
                        <td><?=$p['sids']?></td>
                    </tr>
                <? } ?>
                </table>
            </td>
            <td>&nbsp;</td>
        </tr>

    <? } ?>
</table>