<?php
require_once 'classes/Session.inc';
require_once 'classes/Security.inc';
require_once 'classes/Xajax.inc';
require_once 'classes/Business_Process.inc';

$xajax = new xajax();
$xajax->registerFunction("draw_process");
$xajax->registerFunction("draw_asset_details");

$db = new ossim_db();
$conn = $db->connect();

function draw_process($proc_id)
{
    global $conn;
    $resp = new xajaxResponse();
    $sql = "SELECT name, description FROM bp_process WHERE id = ?";
    $proc_data = $conn->GetRow($sql, array($proc_id));
    if ($proc_data === false) {
        $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
        return $resp;
    }
    $html = '
        <h2 style="text-align: left">'._("Business Process Details").': '.$proc_data['name'].'</h2>
        <br/>
        <table width="70%">
        <tr>
            <th width="70%">'._("Actives").'</th>
            <th>'._("Risk").'</th>
            <th>'._("Priority").'</th>
            <th>'._("Actions").'</th>
        </tr>
    ';
    $sql = "SELECT 
                asset.id,
                asset.name,
                asset.description,
                ref.severity
            FROM bp_asset as asset, bp_process_asset_reference as ref
            WHERE ref.process_id = ? AND asset.id = ref.asset_id";
    if (!$rs = $conn->Execute($sql, array($proc_id))) {
        $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
    } else {
        while (!$rs->EOF) {
            $id = $rs->fields['id'];
            $name = $rs->fields['name'];
            $severity = $rs->fields['severity'];
            $html .= '
            <tr>
                <td>'.$name.'</td>
                <td bgcolor="orange"><b><font color="white">med</font></b></td>
                <td>'.$severity.'</td>
                <td><a href="#" onClick="javascript: xajax_draw_asset_details('.$id.')">(details)</a></td>
            </tr>';
            $rs->MoveNext();
        }
    }
    $html .= '</table>';
    
    $resp->addAssign("asset-info", "style.display", 'none');
    $resp->addAssign("process-info", "style.display", '');
    $resp->addAssign("process-info", "innerHTML", $html);
    return $resp;
}

function draw_asset_details($asset_id)
{
    global $conn;
    $resp = new xajaxResponse();
    $asset = BP_Asset::get($conn, $asset_id);
    $html = '
        <table width="60%" align="left">
        <tr>
            <th width="20%">'._("Asset Name").'</th>
            <td style="text-align: left;"><b>'.$asset->get_name().'</b></td>
        </tr>
        <tr>
            <th>'._("Description").'</th>
            <td style="text-align: left;">'.$asset->get_description().'</td>
        </tr>
        <tr>
            <th colspan="2">'._("Responsibles").'</th>
        </tr>';
    foreach ($asset->get_responsibles() as $responsible) {
        $str = $responsible['name'] . ' ('.$responsible['login'].')';
        $html .= '<tr><td colspan="2" style="text-align: left">'.$str.'</td></tr>';
    }
    $html .= '
    <tr>
        <th width="30%" colspan="2">'._("Members").'</th>
    </tr>';
    foreach ($asset->get_members() as $mem) {
        $str = '<b>'.$mem['type'].'</b>: '.$mem['name'].' - ';
        if ($mem['measure_type'] && $mem['severity']) {
            $str .= $mem['measure_type'].': '.$mem['severity'];
        } else {
            $str .= _('Good');
        }
        $html .= '<tr><td colspan="2" style="text-align: left">'.$str.'</td></tr>';
    }

    $resp->addAssign("asset-info", "style.display", '');
    $resp->AddAssign("asset-info", "innerHTML", $html);
    return $resp;
}

$xajax->setRequestURI($_SERVER["REQUEST_URI"]);
$xajax->processRequests();

//-------------- End Ajax -------------------------//

$sql = "SELECT
            proc.id,
            proc.name,
            count(ref.process_id) as num_assets
        FROM bp_process as proc, bp_process_asset_reference as ref
        WHERE proc.id = ref.process_id
        GROUP BY proc.id";
$procs = $conn->GetAll($sql);
if ($procs === false) {
    die($conn->ErrorMsg());
}

?>
<html>
<head>
<link rel="stylesheet" href="../style/style.css"/>
<?= $xajax->printJavascript('', '../js/xajax.js'); ?>
</head>
<body>
<div id="xajax_debug"></div>
<div id="form_errors"></div>
<h2 style="text-align: left">Summary</h2>

<div style="width: 60%; text-align: right;">
<a href="./bp_edit.php?id=0">(<?=_("Create New Process")?>)</a>&nbsp;
<a href="./asset_list.php">(<?=_("Assets Management")?>)</a>
</div>
<br>
<table width="60%">
<tr>
    <th><?=_("Process Name")?></th>
    <th><?=_("Assets")?></th>
    <th><?=_("Risk")?></th>
    <th><?=_("Actions")?></th>
</tr>
<? foreach ($procs as $p) { ?>
    <tr>
        <td><?=$p['name']?></td>
        <td><?=$p['num_assets']?></td>
        <td bgcolor="orange"><b><font color="white">med</font></b></td>
        <td>
            <a href="#" onClick="javascript: xajax_draw_process(<?=$p['id']?>)">(<?=_("details")?>)</a>&nbsp;
            <a href="./bp_edit.php?id=<?=$p['id']?>">(<?=_("edit")?>)</a>
        </td>
    </tr>
<? } ?>
</table>
<br>

<div id="process-info" style="display: none"></div>
<br><br>

<div id="asset-info" style="display: none"></div>

</body>
</html>