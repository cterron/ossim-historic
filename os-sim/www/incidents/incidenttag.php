<?php
/*
 * Manage TAGS from this a single script. Different states are
 * handled by the $_GET['action'] var. Possible states:
 * 
 * list (default): List TAGs
 * new1step: Form for inserting tag
 * new2step: Values validation and insertion in db
 * delete: Validation and deletion from the db
 * mod1step: Form for updating a tag
 * mod2step: Values validation and update db
 * 
 */


require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
Session::logcheck("MenuIncidents", "IncidentsTags");
require_once 'ossim_db.inc';

require_once 'classes/Incident_tag.inc';

// Avoid the browser resubmit POST data stuff
if (isset($_GET['redirect'])) {
    header('Location: ' . $_SERVER['PHP_SELF']); exit;
}

$db = new ossim_db();
$conn = $db->connect();
$tag = new Incident_tag($conn);

$vals = array(
                'id'    => array(OSS_DIGIT, 'error:'._("ID not valid")),
                'name'  => array(OSS_LETTER, OSS_PUNC, 'error:'._("<b>Name</b> required, should be only letters and underscores")),
                'descr' => array(OSS_TEXT, 'error:'._("<b>Description</b> required and should contain valid characters")) 
             );
$action = !empty($_GET['action']) ? $_GET['action'] : 'list';
$id     = !empty($_GET['id'])     ? $_GET['id']     : null;
$name   = !empty($_POST['name'])  ? $_POST['name']  : null;
$descr  = !empty($_POST['descr']) ? strip($_POST['descr']) : null;

if (in_array($action, array('new2step', 'delete', 'mod2step'))) {

    switch ($action) {
        case 'new2step':
            if (!ossim_valid($name, $vals['name']))   break;
            if (!ossim_valid($descr, $vals['descr'])) break;
            $tag->insert($name, $descr);
            break;
        case 'delete':
            if (!ossim_valid($id, $vals['id'])) break;
            $tag->delete($id);
            break;
        case 'mod2step':
            if (!ossim_valid($id, $vals['id']))       break;
            if (!ossim_valid($name, $vals['name']))   break;
            if (!ossim_valid($descr, $vals['descr'])) break;
            $tag->update($id, $name, $descr);
            break;
    }
    if (ossim_error()) {
        echo ossim_error();
        // back to previous state
        $action = ($action == 'delete') ? 'list' : str_replace('2', '1', $action);

    } else {
        header('Location: ' . $_SERVER['PHP_SELF'] . '?redirect=1'); exit;
    }
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
<h1><?= _("Incident Tags")?> </h1>
<?
/*
 * FORM FOR NEW/EDIT TAG
 */
if ($action == 'new1step' || $action == 'mod1step') {
    if ($action == 'mod1step' && !ossim_error() && ossim_valid($id, $vals['id'])) {
        $f = $tag->get_list("WHERE td.id = $id");
        $name  = $f[0]['name'];
        $descr = $f[0]['descr'];
    }
?>
<form method="post" action="?action=<?=str_replace('1', '2', $action)?>&id=<?=$id?>" name="f">
<table align="center" width="50%">
    <tr>
        <th><?= _("Name") ?></th>
        <td class="left"><input type="input" name="name" size="37" value="<?=$name?>"></td>
    </tr>
    <tr>
        <th><?= _("Description") ?></th>
        <td class="left"><textarea name="descr" cols="35" rows="15"><?=$descr?></textarea></td>
    </tr>
    <tr><th colspan="2" align="center">
        <input type="submit" value="OK">&nbsp;
        <input type="button" onClick="document.location = '<?=$_SERVER['PHP_SELF']?>'" value="<?=_("Cancel")?>">
    </th></tr>
</table>    
</form>
<script>document.f.name.focus();</script>
<?
/*
 * LIST TAGS
 */

} elseif ($action == 'list') {
?>
<table align="center" width="70%">
    <tr>
        <th><?= _("Id") ?></th>
        <th><?= _("Name") ?></th>
        <th><?= _("Description") ?></th>
        <th><?= _("Actions") ?></th>
    </tr>
<? foreach ($tag->get_list() as $f) { ?>
    <? //printr($f); exit; ?>
    <tr>
        <td valign="top"><b><?= $f['id']?></b></td>
        <td valign="top" style="text-align: left;" NOWRAP><?= htm($f['name'])?></td>
        <td valign="top" style="text-align: left;"><?= htm($f['descr'])?></td>
        <td NOWRAP> 
            [<a href="?action=mod1step&id=<?= $f['id']?>">Modify</a>]&nbsp;
            [<a href="?action=delete&id=<?= $f['id']?>"
              <? if ($f['num'] >= 1) { ?>
              onClick="return confirm('<?printf(_("There are %d incidents using this tag. Do you really want to delete it?"), $f['num'])?>');" 
              <? } ?>
             >Delete</a>]
        </td>
    </tr>
<? } ?>
    <tr><th colspan="4" align="center">
        <a href="?action=new1step"><?= _("Add new tag")?></a>
    </th></tr>
</table>

<?
}
?>