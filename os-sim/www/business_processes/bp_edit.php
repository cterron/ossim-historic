<?php
require_once 'classes/Session.inc';
require_once 'classes/Security.inc';
require_once 'classes/Xajax.inc';
require_once 'classes/Business_Process.inc';

$xajax = new xajax();
$xajax->registerFunction("draw_assets");
$xajax->registerFunction("add_asset");
$xajax->registerFunction("remove_asset");
$xajax->registerFunction("draw_asset_details");
$xajax->registerFunction("change_asset_severity");
$xajax->registerFunction("edit_process");

$db = new ossim_db();
$conn = $db->connect();

$id = GET('id');
if (!ossim_valid($id, OSS_DIGIT, 'illegal:ID')) {
    die(ossim_error());
}

function draw_assets($selected_value)
{
    global $conn, $id;
    $resp = new xajaxResponse();
    $actives = array(
        0 => array('active name id1', '1', 'low'),
        1 => array('active name id2', '2', 'high'),
    );
    // insert new row and retrieve full person data
    if (is_array($selected_value) && $id = current($selected_value)) {
        $actives[3] = array('new active', $id);
    }
    $tpl = '
    <div class="row">
        <div class="col1">%ACTIVE%</div>
        <div class="col2" onChange="javascript: xajax_change_asset_severity(%ID%, xajax.getFormValues(\'bp_form\', true, \'prio_active_%ID%\'))">
            <select name="prio_active_%ID%">
            <option value="0" %0%>'._("Low").'</option>
            <option value="1" %1%>'._("Medium").'</option>
            <option value="2" %2%>'._("High").'</option>
            </select>&nbsp;
            <a onClick="javascript: xajax_remove_asset(%ID%)">('._("remove").')</a>&nbsp;
            <a onClick="javascript: xajax_draw_asset_details(%ID%)">('._("details").')</a>
        </div>
    </div><hr>';
    $sql = "SELECT
                bp_asset.id,
                bp_asset.name,
                bp_asset.description,
                 bp_process_asset_reference.severity
            FROM bp_asset, bp_process_asset_reference
            WHERE
                bp_process_asset_reference.process_id = ? AND
                bp_process_asset_reference.asset_id = bp_asset.id
            GROUP BY bp_asset.id
            ORDER BY bp_asset.name";
    $assets = $conn->GetAll($sql, array($id));
    if ($assets === false) {
        $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
        return $resp;
    }
    $html = '';
    foreach ($assets as $a) {
        $tmp = str_replace('%ACTIVE%', $a['name'], $tpl);
        foreach (array(0, 1, 2) as $prio) {
            $selected = ($a['severity'] == $prio) ? 'selected' : '';
            $replace  = '%'.$prio.'%';
            $tmp = str_replace($replace, $selected, $tmp);
        }
        $html .= str_replace('%ID%', $a['id'], $tmp);
    }
    $resp->addAssign("assets", "innerHTML", $html);
    $resp->addAssign("html_assets_select", "innerHTML", html_assets_select());
    return $resp;
}

/*
 * Returns the html needed to generate the SELECT html element with
 * the list of assets not assigned to the current process id.
 * 
 * This function is only called by draw_assets()
 */
function html_assets_select()
{
    global $conn, $id;
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
    WHERE ref.process_id != ? OR ref.process_id IS NULL
    ORDER BY asset.name";
    if (!$rs = $conn->Execute($sql, array($id))) {
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
    $html = '<select name="bp_new_asset">';
    $belongs = '';
    foreach ($assets as $aid => $data) {
        unset($belongs);
        if (isset($asset_ref[$aid])) {
            foreach ($asset_ref[$aid] as $proc_id) {
                $belongs[] = $procs[$proc_id];
            }
            $belongs = implode(', ', $belongs);
        } else {
            $belongs = _("not assigned");
        }
        $html .= "<option value='$aid'>".$data['asset_name']." (".$belongs.")</option>";
    }
    $html .= '</select>
    <input type="button" value="'._("Add").'"
           onClick="javascript: xajax_add_asset(xajax.getFormValues(\'bp_form\', true, \'bp_new_asset\'))">&nbsp;
    <input type="button" value="'._("Details").'"
            onClick="javascript: xajax_draw_asset_details(xajax.getFormValues(\'bp_form\', true, \'bp_new_asset\'))">';
    return $html;
}

function remove_asset($asset_id)
{
    // remove reference from db
    global $conn, $id;
    $sql = "DELETE FROM bp_process_asset_reference
            WHERE process_id = ? AND asset_id = ?";
    if (!$conn->Execute($sql, array($id, $asset_id))) {
        die($conn->ErrorMsg());
    }
    return draw_assets(false);
}

/*
 * @param $severity comes from xajax in the form: Array ( [prio_active_4] => 2 )
 */
function change_asset_severity($asset_id, $severity)
{
    global $conn, $id;
    $resp = new xajaxResponse();
    //xajax_debug($asset_id, $resp);
    $s = current($severity);
    $sql = "UPDATE bp_process_asset_reference
            SET severity = ?
            WHERE process_id = ? AND asset_id = ?";
    if (!$conn->Execute($sql, array($s, $id, $asset_id))) {
        $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
    }
    return $resp;
}

function draw_asset_details($asset_id)
{
    global $conn;
    $resp = new xajaxResponse();
    $asset = BP_Asset::get($conn, $asset_id);
    $html = '
        <table width="60%" align="center">
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
        $str = '<b>'.$mem['type'].'</b>: '.$mem['name'];
        $html .= '<tr><td colspan="2" style="text-align: left">'.$str.'</td></tr>';
    }

    $resp->addAssign("asset-info", "style.display", '');
    $resp->AddAssign("asset-info", "innerHTML", $html);
    return $resp;
}

function add_asset($asset_id)
{
    global $conn, $id;
    //$resp = new xajaxResponse();
    //xajax_debug($asset_id, $resp);
    //return $resp;
    $sql = "INSERT INTO bp_process_asset_reference
            (process_id, asset_id, severity) VALUES (?, ?, ?)";
    $params = array($id, $asset_id['bp_new_asset'], 1);
    if (!$conn->Execute($sql, $params)) {
        die($conn->ErrorMsg());
    }
    return draw_assets(false);
}

function edit_process($form_data)
{
    global $conn, $id;
    $resp = new xajaxResponse();
    ossim_valid($form_data['bp_name'], OSS_INPUT, 'illegal:'._("Name"));
    ossim_valid($form_data['bp_desc'], OSS_TEXT, 'illegal:'._("Description"));
    if (ossim_error()) {
        $resp->AddAssign("form_errors", "innerHTML", ossim_error());
    } else {
        if ($id == 0) {
            $sql = "INSERT INTO bp_process (id, name, description) VALUES (?, ?, ?)";
            $id = $conn->GenID('bp_seq');
            $params = array($id, $form_data['bp_name'], $form_data['bp_desc']);
            if (!$conn->Execute($sql, $params)) {
                $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
            } else {
                $resp->addRedirect($_SERVER['PHP_SELF']."?id=$id");
            }
        } else {
            $sql = "UPDATE bp_process SET name=?, description=? WHERE id=?";
            $params = array($form_data['bp_name'], $form_data['bp_desc'], $id);
            if (!$conn->Execute($sql, $params)) {
                $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
            } else {
                $resp->addRedirect("./bp_list.php");
            }
        }
    }
    return $resp;
}

$xajax->setRequestURI($_SERVER["REQUEST_URI"]);
$xajax->processRequests();

//-------------- End Ajax -------------------------//

if ($id != 0) {
    $sql = "SELECT name, description
            FROM bp_process
            WHERE id = ?";
    $proc_data = $conn->GetRow($sql, array($id));
    if ($proc_data === false) {
        die($conn->ErrorMsg());
    }
} else {
    $proc_data['name'] = '';
    $proc_data['description'] = '';
}
?>
<html>
<head>
  <title><?_("Business Processes")?></title>
  <link rel="stylesheet" href="../style/style.css"/>
<?= $xajax->printJavascript('', '../js/xajax.js'); ?>
<style type="text/css">
    .contents {
        width: 60%;
    }
    .row {
        clear: both;
    }
    .col1 {
        float: left;
        width: 50%;
    }
    .col2 {
        float: left;
        text-align: right;
        width: 50%;
    }
    hr {
        clear: both;
    }
</style>
  
</head>
<body>
<div id="xajax_debug"></div>
<div id="form_errors"></div>
<form id="bp_form">
<table width="100%" align="center"><tr><td style="border-width: 0px">
<table width="70%" style="border-width: 0px">
<tr>
    <th><?=_("Name")?></th>
    <td style="text-align: left; border-width: 0px"><input type="text" size="40" name="bp_name" value="<?=$proc_data['name']?>"></td>
</tr>
<tr>
    <th><?=_("Description")?></th>
    <td style="text-align: left; border-width: 0px">
        <textarea NAME="bp_desc" COLS="40" ROWS=5 WRAP=HARD><?=$proc_data['description']?></textarea>
    </td>
</tr>
</table>
<br>
<? if ($id == 0) { ?>
    <center>
    <input type="button" value="<?=_("Continue")?>"
           onClick="javascript: xajax_edit_process(xajax.getFormValues('bp_form'))">
    </center>
<?
} else {
?>
<table width="60%"><tr><th><?=_("Assets")?></th></tr></table>
<div id="assets" class="contents">
<!-- Filled by draw_responsibles() -->
</div>
<script>xajax_draw_assets(false)</script>
<br>
<div id="html_assets_select" class="row" style="text-align: center; width: 60%">
<!-- Filled by draw_assets(), generated at html_assets_select() -->
</div>
<br>
</form>
</td><td>
<input type="button" value="<?=_("Continue")?>-&gt;"
       onClick="javascript: xajax_edit_process(xajax.getFormValues('bp_form'))">
</tr></table>
<? } ?>
<br><br>
<div id="asset-info" style="display: none; text-align: center">
</div>

</body>
</html>