<?php
require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
require_once 'ossim_db.inc';
require_once 'classes/Plugin.inc';
require_once 'classes/Plugingroup.inc';

Session::logcheck("MenuPolicy", "PolicyPluginGroups");

$db = new ossim_db();
$conn = $db->connect();

$plugin_list = Plugin::get_list($conn, "ORDER BY name");

if (GET('action') == 'edit') {
    $group_id = GET('id');
    ossim_valid($group_id, OSS_DIGIT, 'illegal:ID');
    if (ossim_error()) {
        die(ossim_error());
    }
    $where = "plugin_group_descr.group_id=$group_id";
    $list = Plugingroup::get_list($conn, $where);
    if (count($list) != 1) {
        die("Invalid ID");
    }
    $plug_ed = $list[0];
    $name = $plug_ed->get_name();
    $descr = $plug_ed->get_description();
    $plugs = $plug_ed->get_plugins();
} else {
    $group_id = $name = $descr = null;
    $plugs = array();
}

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

function toggle_plugin(id)
{
    var check = $('check' + id);
    if (check.checked) {
       Element.show('editsid'+id);
       Element.setStyle('plugin'+id, {background: '#CFCFCF'});
   } else {
       $('sid'+id).value = '';
       Element.hide('editsid'+id);
       Element.hide('errorsid'+id);
       Element.setStyle('plugin'+id, {background: 'white'});
   }
}

function validate_sids_str(id)
{
    var sids_str = $('sid'+id).value;
    new Ajax.Updater (
        'error'+id, // Element to refresh
        'modifyplugingroups.php?interface=ajax&method=validate_sids_str', // URL
        {          // options
            method: 'get',
            asynchronous: true,
            parameters: 'sids_str='+sids_str,
            onComplete: function(req) {
                if (req.responseText) {
                    Element.show('errorsid'+id);
                    $('errorsid'+id).innerHTML = req.responseText+'<br/>';
                } else {
                    Element.hide('errorsid'+id);
                }
            }
        }
    );
    return false;
}

</script>
<form id="myform" name="myform" action="modifyplugingroups.php?action=<?=GET('action')?>&id=<?=$group_id?>" method="POST">
<center><input type="submit" value="<?=_("Accept")?>"></center>
<br>
<table align="center" width="95%">
    <tr>
        <th width="10%"><?=_("Group ID")?></th>
        <th width="25%"><?=_("Name").required()?></th>
        <th width="70%"><?=_("Description").required()?></th>
    </tr>
    <tr>
        <td class="noborder"><b><?=$group_id?></b>&nbsp;</td>
        <td class="noborder">
            <input type="text" name="name" value="<?=$name?>" size="30">
        </td>
        <td class="noborder">
          <textarea name="descr" rows="2" cols="50" wrap="on"><?=$descr?></textarea>
        </td>
    </tr>
    <tr>
        <td class="noborder" colspan="3">
        
        <table width="100%">
        <? foreach ($plugin_list as $plugin) {
               $id = $plugin->get_id();
               if (array_key_exists($id, $plugs)) {
                $checked = 'checked';
                $sids  = $plugs[$id]['sids'];
               } else {
                $checked = '';
                $sids  = '';
               }
        ?>
            <tr id="plugin<?=$id?>">
                <td width="1%" style="text-align: left;">
                    <input id="check<?=$id?>" type="checkbox" name=""
                           onClick="javascript: toggle_plugin('<?=$id?>');"
                           <?=$checked?>>
                </td>
                <td width="1%"><?= $id ?></td>
                <td width="1%" style="text-align: left;"><b><?= $plugin->get_name() ?></b></td>
                <td width="1%" style="text-align: left;" NOWRAP><?= $plugin->get_description()?></td>
                <td style="text-align: left;">
                    <span id="errorsid<?=$id?>" style="background: red; display: none"></span>
                    <span id="editsid<?=$id?>" style="display: none;" NOWRAP>
                        <b>SIDs</b>:&nbsp;
                        <input id="sid<?=$id?>"
                                 type="text" 
                                 name="sids[<?=$id?>]"
                                 value="<?=$sids?>"
                                 size="48">&nbsp;
                        <a href="#" onClick="javascript: return validate_sids_str('<?=$id?>')">&lt;-</a>
                    </span>&nbsp;
                    
                </td>
            </tr><script>toggle_plugin('<?=$id?>');</script>
        <? } ?>
        </table>
        
        </td>
    </tr>
</table>
<br>
<center><input type="submit" value="<?=_("Accept")?>"></center>
</form>
