<?php
require_once 'classes/Session.inc';
require_once 'classes/Security.inc';
require_once 'classes/Xajax.inc';
require_once 'classes/Util.inc';

$db = new ossim_db();
$conn = $db->connect();

$sql = "
    SELECT 
        asset.id as asset_id,
        asset.name as asset_name,
        asset.description as asset_description,
        proc.id as proc_id,
        proc.name as proc_name,
        proc.description AS proc_description
    FROM
        bp_asset AS asset
    LEFT JOIN bp_process_asset_reference AS ref ON asset.id = ref.asset_id
    LEFT JOIN bp_process AS proc ON ref.process_id = proc.id
    ORDER BY asset.name";
if (!$rs = $conn->Execute($sql)) {
    die($conn->ErrorMsg());
}
$assets = $procs = $asset_ref = array();
while (!$rs->EOF) {
    $aid = $rs->fields['asset_id'];
    $pid = $rs->fields['proc_id'];
    
    if (!empty($pid)) {
        $asset_ref[$aid][] = $pid;
        $procs[$rs->fields['proc_id']] = $rs->fields['proc_name'];
    }
    if (!isset($assets[$aid])) {
        $assets[$aid] = $rs->fields;
    }
    $rs->MoveNext();
}

?>
<html>
<head>
<link rel="stylesheet" href="../style/style.css"/>
</head>
<body>
<br>
<div style="width: 100%; text-align: right;">
<a href="./bp_list.php">(<?=_("Back to Business Process View")?>)</a>&nbsp;
<a href="./asset_edit.php?id=0">(<?=_("Create New Asset")?>)</a>&nbsp;
</div><br>
<table width="90%" align="center">
<tr><th><?=_("Search assets")?></th></tr>
<tr>
    <td><?=_("Belongs to process")?>: <select name="proc_view">
            <option value="all"><?=_("All")?></option>
            <option value="none"><?=_("None")?></option>
            <? foreach ($procs as $pid => $pname) { ?>
                <option value="<?=$pid?>"><?=$pname?></option>
            <? } ?>
        </select>&nbsp;
        <?=_("Name contains")?>: <input type="text" name="proc_name" size="20">&nbsp;
        <input type="button" value="<?=_("Search")?>">
    </td>
</tr>
</table>

<br>
<table width="60%" align="center">
<tr>
    <th><?=_("Asset Name")?></th>
    <th><?=_("Belongs to")?></th>
    <th><?=_("Actions")?></th>
</tr>
<? foreach ($assets as $aid => $a) { ?>
    <tr>
        <td><?=$a['asset_name']?></td>
        <td>
        <?
        if (isset($asset_ref[$aid])) {
            foreach ($asset_ref[$aid] as $pid) {    
        ?>
            - <?=$procs[$pid]?><br>
        <?  }
        } else { ?>
            &nbsp;
        <? } ?>
        </td>
        <td><a href="./asset_edit.php?id=<?=$a['asset_id']?>">(<?=_("edit")?>)</a>
    </tr>
<? } ?>
</table>
</body></html>