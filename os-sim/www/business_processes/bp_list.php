<?php
require_once 'classes/Session.inc';
require_once 'classes/Security.inc';
require_once 'classes/Xajax.inc';
require_once 'classes/Business_Process.inc';
require_once 'classes/Incident.inc'; // some functions here used in bp_member_status_color()
Session::logcheck("MenuControlPanel", "BusinessProcesses");

if (Session::menu_perms("MenuControlPanel", "BusinessProcessesEdit")) {
    $can_edit = true;
} else {
    $can_edit = false;
}

$xajax = new xajax();
$xajax->registerFunction("draw_process");
$xajax->registerFunction("draw_asset_details");
$xajax->registerFunction("delete_process");

$db = new ossim_db();
$conn = $db->connect();

function delete_process($proc_id)
{
    global $conn;
    $error = false;
    $resp = new xajaxResponse();
    $sql = "DELETE FROM bp_process_asset_reference WHERE process_id=?";
    if (!$conn->Execute($sql, array($proc_id))) {
        $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
        $error = true;
    }
    $sql = "DELETE FROM bp_process WHERE id=?";
    if (!$conn->Execute($sql, array($proc_id))) {
        $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
        $error = true;
    }
    if ($error) {
        return $resp;
    }
    $resp->AddRedirect($_SERVER['PHP_SELF']);
    return $resp;
}

function draw_process($proc_id)
{
    global $conn, $can_edit;
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
            <th width="70%">'._("Assets").'</th>
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
            FROM
                bp_asset as asset,
                bp_process_asset_reference as ref
            WHERE ref.process_id = ? AND asset.id = ref.asset_id";
    if (!$rs = $conn->Execute($sql, array($proc_id))) {
        $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
    } else {
        while (!$rs->EOF) {
            $id = $rs->fields['id'];
            $name = $rs->fields['name'];
            switch ($rs->fields['severity']) {
            	case "0": $severity = _("Low"); break;
                case "1": $severity = _("Medium"); break;
                case "2": $severity = _("High"); break;
                default:  $severity = _("n/a");
            }
            $risk = get_asset_risk($id);
            //xajax_debug($risk, $resp);
            list($fgcolor, $bgcolor, $str) = get_status_details($risk);
            $html .= '
            <tr>
                <td>'.$name.'</td>
                <td bgcolor="'.$bgcolor.'"><font color="'.$fgcolor.'"><b>'.$str.'</b></font></td>
                <td>'.$severity.'</td>
                <td>
                <a href="#" onClick="javascript: xajax_draw_asset_details('.$id.')">('._("details").')</a>&nbsp;';
           if ($can_edit) {
                $html .= '<a href="./asset_edit.php?id='.$id.'&referrer=bp_list">('._("edit").')</a>';
           }
           $html .= '
                </td>
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
/*
 * @param int $status - Number between 0-10: 0 = ok, 2 = low, 5 = med, 7 = high
 * (we use the same metric as Incidents for this)
 */
function bp_member_status_html($status)
{
    if ($status === "0") {
        $bgcolor = 'white';
        $fgcolor = 'green';
        $status_str = _("Ok");
    } elseif ($status === null) {
        $bgcolor = 'red';
        $fgcolor = 'white';
        $status_str = _("n/a");
    } else {
        $bgcolor = Incident::get_priority_bgcolor($status);
        $fgcolor = Incident::get_priority_fgcolor($status);
        $status_str = Incident::get_priority_string($status);
    }
    $html = "<span style='border: 1px solid $bgcolor; background: $bgcolor; color: $fgcolor'>$status_str</span>";
    return $html;
}

function get_process_risk($proc_id)
{
    global $conn;
    $sql = "SELECT 
                asset.id,
                asset.name,
                ref.severity
            FROM
                bp_asset as asset,
                bp_process_asset_reference as ref
            WHERE ref.process_id = ? AND asset.id = ref.asset_id";
    if (!$rs = $conn->Execute($sql, array($proc_id))) {
        die($conn->ErrorMsg());
    }
    /*
     * Process:
     * 1) We use a 0 to 10 scale. Composed by: base risk + aggregated risk
     * 2) Base risk: Calculate the worse risk, that's the higher risk between
     * the higher priority assets. Max base risk in the scale is determined by
     * the priority of that asset. Low: 3, Med: 6, High: 9. And the actual
     * value in the scale depends on the formula (risk * max_base_risk / 10)
     */
    $data = array();
    while (!$rs->EOF) {
        $id = $rs->fields['id'];
        $name = $rs->fields['name'];
        $priority = $rs->fields['severity'];
        $risk = get_asset_risk($id);
        $data[$priority][] = round($risk);
        $rs->MoveNext();
    }
    // No assets assigned yet
    if (!count($data)) {
        return null;
    }
    $max_priority = max(array_keys($data));
    $max_risk = max($data[$max_priority]);
    //printr($data);
    //printr($max_priority); printr($max_risk);
    return $max_risk;
}

function get_asset_risk($asset_id)
{
    global $conn;
    $asset = BP_asset::get($conn, $asset_id);
    $members = $asset->get_members();
    $times = $total = 0;
    foreach ($members as $member) {
        // if no measure type o no measure value take that as high risk
        // XXX review that assertion
        if ($member['measure_type'] === null || $member['severity'] === null) {
            $severity = 10;
        } else {
            $severity = $member['severity'];
        }
        $total += $severity;
        $times++;
    }
    return $times == 0 ? null : $total/$times;
}

function get_status_details($status)
{
    if ($status === null) {
        $bgcolor = 'white';
        $fgcolor = 'red';
        $status_str = _("n/a");
    } elseif ((int)$status === 0) {
        $bgcolor = 'white';
        $fgcolor = 'green';
        $status_str = _("Ok");
    } else {
        $bgcolor = Incident::get_priority_bgcolor($status+1);
        $fgcolor = 'white';
        $status_str = Incident::get_priority_string($status);
    }
    return array($fgcolor, $bgcolor, $status_str);
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
    $times = 0;
    foreach ($asset->get_responsibles() as $responsible) {
        $str = '';
        if ($responsible['email']) {
            $str = '<a href="mailto:'.$responsible['email'].'?subject='.$asset->get_name().'"><img border="0" src="../pixmaps/email_icon.gif"></a>&nbsp;';
        }
        $str .= $responsible['name'] . ' ('.$responsible['login'].')';
        $html .= '<tr><td colspan="2" style="text-align: left">'.$str.'</td></tr>';
        $times++;
    }
    if (!$times) {
        $html .= '<tr><td colspan="2" style="text-align: left"><i>'._("None set").'</i></td></tr>';
    }
    $html .= '
    <tr>
        <th width="30%" colspan="2">'._("Status of Members").'</th>
    </tr>
    <tr>
        <td colspan="2">
            <table width="100%">
                <tr>
                    <th>Member type</th>
                    <th>Member</th>
                    <th>Measure type</th>
                    <th>Severity</th>
                </tr>
            ';
    foreach ($asset->get_members() as $mem) {
        $html .= 
                '<tr valign="center">
                    <td>'.$mem['type'].'</td>
                    <td>'.$mem['name'].'</td>
                    <td>'.BP_Asset::get_measure_type_str($mem['measure_type']).'</td>
                    <td><b>'.bp_member_status_html($mem['severity']).'</b></td>
                </tr>';
    }
    $html .= '</table></td></tr></table>';
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
        FROM
            bp_process as proc
        LEFT JOIN bp_process_asset_reference AS ref ON proc.id = ref.process_id
        GROUP BY proc.id";
$procs = $conn->GetAll($sql);
if ($procs === false) {
    die($conn->ErrorMsg());
}
//$foo = get_process_risk(11);
//echo "total: $foo<br>";
?>
<html>
<head>
<link rel="stylesheet" href="../style/style.css"/>
<?= $xajax->printJavascript('', XAJAX_JS); ?>
</head>
<body>
<div id="form_errors"></div>
<h2 style="text-align: left"><?=_("Summary")?></h2>
<div style="width: 60%; text-align: right;">
<? if ($can_edit) { ?>
    <a href="./bp_edit.php?id=0">(<?=_("Create New Process")?>)</a>&nbsp;
<? } ?>
<a href="./asset_list.php">(<?=_("Assets Management")?>)</a>
</div>
<br>
<table width="60%">
<tr>
    <th><?=_("Process Name")?></th>
    <th><?=_("Num. Assets")?></th>
    <th><?=_("Risk")?></th>
    <th><?=_("Actions")?></th>
</tr>
<? foreach ($procs as $p) { ?>
    <tr>
        <td><?=$p['name']?></td>
        <td><?=$p['num_assets']?></td>
        <?
        $risk = get_process_risk($p['id']);
        list($fgcolor, $bgcolor, $str) = get_status_details($risk);
        ?>
        <td bgcolor="<?=$bgcolor?>"><b><font color="<?=$fgcolor?>"><?=$str?></font></b></td>
        <td>
            <a href="#" onClick="javascript: xajax_draw_process(<?=$p['id']?>)">(<?=_("details")?>)</a>
            <? if ($can_edit) { ?>
            &nbsp;<a href="./bp_edit.php?id=<?=$p['id']?>">(<?=_("edit")?>)</a>&nbsp;
            <a href="#" onClick="javascript: xajax_delete_process(<?=$p['id']?>)">(<?=_("delete")?>)</a>
            <? } ?>
        </td>
    </tr>
<? } ?>
</table>
<br>

<div id="process-info" style="display: none"></div>
<br><br>

<div id="asset-info" style="display: none"></div>
<div id="xajax_debug" style="width: 100%"></div>
<br>&nbsp;<br>
</body>
</html>
