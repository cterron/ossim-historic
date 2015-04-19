<?php
require_once 'classes/Session.inc';
require_once 'classes/Security.inc';
require_once 'classes/Xajax.inc';
require_once 'classes/Util.inc';
Session::logcheck("MenuControlPanel", "BusinessProcesses");
Session::logcheck("MenuControlPanel", "BusinessProcessesEdit");

$db = new ossim_db();
$conn = $db->connect();

$proc_id = GET('proc_id');
$referrer = GET('referrer');

$id = GET('id');
if (!ossim_valid($id, OSS_DIGIT, 'illegal:ID')) {
    die(ossim_error());
}

$xajax = new xajax();
$xajax->registerFunction("draw_responsibles");
$xajax->registerFunction("remove_responsible");
$xajax->registerFunction("edit_asset");
$xajax->registerFunction("draw_members");
$xajax->registerFunction("remove_member");
$xajax->registerFunction("draw_members_select");

function draw_responsibles($selected_value)
{
    global $id, $conn;
    $resp = new xajaxResponse();

    // insert new row and retrieve full person data
    if (is_array($selected_value) && $login = current($selected_value)) {
        ossim_valid($login, OSS_USER, 'illegal:'._("User login"));
        if (ossim_error()) {
            $resp->AddAssign("form_errors", "innerHTML", ossim_error());
        } else {
            $sql = "INSERT INTO bp_asset_responsible (asset_id, login) VALUES (?, ?)";
            $params = array($id, $login);
            if (!$conn->Execute($sql, $params)) {
                $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
            }
        }
    }
    // retrieve from db ordered by name
    $persons = get_users($id);
    if ($persons === false) {
        $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
        return $resp;
    }
    $tpl = '
    <div class="row">
        <div class="col1">%NAME%</div>
        <div class="col2">
        <a onClick="javascript: xajax_remove_responsible(\'%LOGIN%\')">('._("remove").')</a>
        </div>
    </div><hr>
    ';
    $html = '';
    foreach ($persons as $p) {
        $tmp = str_replace('%NAME%', $p[1]." (".$p[0].")", $tpl);
        $html .= str_replace('%LOGIN%', $p[0], $tmp);
    }
    $resp->addAssign("responsibles", "innerHTML", $html);
    $resp->addAssign("responsibles", "style.display", '');
    return $resp;
}

function draw_members($form_data)
{
    global $id, $conn;
    $resp = new xajaxResponse();
    // insert new member
    if (is_array($form_data)) {
        ossim_valid($form_data["member_type"], OSS_LETTER, OSS_SCORE, OSS_DOT, 'illegal:'._("Member Type"));
        ossim_valid($form_data["member_name"], OSS_INPUT, 'illegal:'._("Member Name"));
        if (ossim_error()) {
            $resp->AddAssign("form_errors", "innerHTML", ossim_error());
        } else {
            // Check for duplicates
            $sql = "SELECT member FROM bp_asset_member WHERE asset_id=? AND member=? AND member_type=?";
            if (!$rs = $conn->Execute($sql, array($id, $form_data["member_name"], $form_data["member_type"]))) {
                $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
                return $resp;
            }
            if ($rs->EOF) {
                $sql = "INSERT INTO bp_asset_member (asset_id, member, member_type) " .
                       "VALUES (?, ?, ?)";
                $params = array($id, $form_data["member_name"], $form_data["member_type"]);
                if (!$conn->Execute($sql, $params)) {
                    $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
                    return $resp;
                }
            }
            $resp->AddAssign("form_errors", "innerHTML", '');
        }
    }
    
    // display members
    $sql = "SELECT member, member_type " .
           "FROM bp_asset_member " .
           "WHERE asset_id=?";
    $members = $conn->GetAll($sql, array($id));
    if ($members === false) {
        $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
        return $resp;
    }
    $tpl = '
    <div class="row">
        <div class="col1"><b>%TYPE%</b>: %NAME%</div>
        <div class="col2">
        <a onClick="javascript: xajax_remove_member(\'%JNAME%\', \'%JTYPE%\')">('._("remove").')</a>
        </div>
    </div><hr>
    ';
    $html = '';
    foreach ($members as $i => $m) {
        $tmp = str_replace('%TYPE%', $m[1], $tpl);
        $tmp = str_replace('%JTYPE%', Util::string2js($m[1]), $tmp);
        $tmp = str_replace('%NAME%', $m[0], $tmp);
        $html .= str_replace('%JNAME%', Util::string2js($m[0]), $tmp);
    }
    $resp->addAssign("members", "innerHTML", $html);
    $resp->addAssign("members", "style.display", '');
    
    return $resp;
}

function remove_responsible($login)
{
    global $id, $conn;
    // remove reference from db
    $sql = "DELETE FROM bp_asset_responsible
            WHERE asset_id=? AND login=?";
    $params = array($id, $login);
    if (!$conn->Execute($sql, $params)) {
        die($conn->ErrorMsg());
    }
    return draw_responsibles(false);
}

function remove_member($name, $type)
{
    global $id, $conn;
    $sql = "DELETE FROM bp_asset_member " .
           "WHERE asset_id=? AND member=? AND member_type=?";
    $params = array($id, $name, $type);
    if (!$conn->Execute($sql, $params)) {
        die($conn->ErrorMsg());
    }
    return draw_members(false);
}

function edit_asset($form_data)
{
    global $conn, $id, $proc_id, $referrer;
    $resp = new xajaxResponse();
    ossim_valid($form_data['bp_name'], OSS_INPUT, 'illegal:'._("Name"));
    ossim_valid($form_data['bp_desc'], OSS_TEXT, 'illegal:'._("Description"));
    if (ossim_error()) {
        $resp->AddAssign("form_errors", "innerHTML", ossim_error());
    } else {
        // Check if there is already an asset with that name
        $sql = "SELECT name FROM bp_asset WHERE name=?";
        if ($id != 0) {
            $sql .= " AND id <> $id";
        }
        $params = array($form_data['bp_name']);
        if (!$rs = $conn->Execute($sql, $params)) {
            $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
            return $resp;
        } elseif (!$rs->EOF) {
            $resp->AddAssign("form_errors", "innerHTML", ossim_error(_("There is already an asset with that name")));
            return $resp;
        }
        // New record
        if ($id == 0) {
            $sql = "INSERT INTO bp_asset (id, name, description) VALUES (?, ?, ?)";
            $id = $conn->GenID('bp_seq');
            $params = array($id, $form_data['bp_name'], $form_data['bp_desc']);
            if (!$conn->Execute($sql, $params)) {
                $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
            } else {
                $resp->addRedirect($_SERVER['PHP_SELF']."?id=$id&proc_id=$proc_id");
            }
        // Continue button, reflect possible changes in name or description
        } else {
            $sql = "UPDATE bp_asset SET name=?, description=? WHERE id=?";
            $params = array($form_data['bp_name'], $form_data['bp_desc'], $id);
            if (!$conn->Execute($sql, $params)) {
                $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
            } elseif ($referrer == 'bp_list') {
                $resp->addRedirect("./bp_list.php");
            } elseif ($proc_id) {
                $resp->addRedirect("./bp_edit.php?id=$proc_id");
            } else {
                $resp->addRedirect("./asset_list.php");
            }
        }
    }
    return $resp;
}

//
// @returns array or false in case of db error
//
function get_users($asset_id = null)
{
    global $conn;
    if ($asset_id) {
        $sql = "SELECT users.login, users.name
                FROM
                    users, bp_asset_responsible
                WHERE
                    users.login = bp_asset_responsible.login AND
                    bp_asset_responsible.asset_id = ?
                ORDER BY users.name";
        $params = array($asset_id);
    } else {
        $sql = "SELECT login, name FROM users ORDER BY name";
        $params = array();
    }
    return $conn->getAll($sql, $params);
}

function draw_members_select($form_data)
{
    global $conn, $id;
    $resp = new xajaxResponse();
    $type = $form_data['member_type'];
    // The user selected the empty type
    if (!$type) {
        $resp->AddAssign("members_select", "innerHTML", _("Please select a type"));
        return $resp;
    }
    //
    // Get the list of members of the given type
    //
    $options = array();
    switch ($type) {
        case 'host':
            include_once 'classes/Host.inc';
            $list = Host::get_list($conn, null, 'ORDER BY hostname');
            foreach ($list as $obj) {
                $descr = $obj->get_descr();
                if (strlen($descr) > 50) {
                    $descr = substr($descr, 0, 47) . '...';
                }
                $options[$obj->get_ip()] = $obj->get_hostname().' '.$obj->get_ip().' - '.$descr;
            }
            break;
        case 'net':
            include_once 'classes/Net.inc';
            $list = Net::get_list($conn, 'ORDER BY name');
            foreach ($list as $obj) {
                $descr = $obj->get_descr();
                if (strlen($descr) > 50) {
                    $descr = substr($descr, 0, 47) . '...';
                }
                $options[$obj->get_name()] = $obj->get_name().' '.$obj->get_ips().' - '.$descr;
            }
            break;
        case 'host_group':
            include_once 'classes/Host_group.inc';
            $list = Host_group::get_list($conn, 'ORDER BY name');
            foreach ($list as $obj) {
                $descr = $obj->get_descr();
                if (strlen($descr) > 50) {
                    $descr = substr($descr, 0, 47) . '...';
                }
                $options[$obj->get_name()] = $obj->get_name().' - '.$descr;
            }
            break;
        case 'net_group':
            include_once 'classes/Net_group.inc';
            $list = Net_group::get_list($conn, 'ORDER BY name');
            foreach ($list as $obj) {
                $descr = $obj->get_descr();
                if (strlen($descr) > 50) {
                    $descr = substr($descr, 0, 47) . '...';
                }
                $options[$obj->get_name()] = $obj->get_name().' - '.$descr;
            }
            break;
    }
    //
    // Build the SELECT tag
    //
    $html = '<select name="member_name">';
    foreach ($options as $name => $description) {
        $html .= "<option value='$name'>$description</option>";
    }
    $html .= '</select>';
    $resp->AddAssign("members_select", "innerHTML", $html);
    return $resp;
}

$xajax->setRequestURI($_SERVER["REQUEST_URI"]);
$xajax->processRequests();

//-------------- End Ajax -------------------------//  

$bp_name = $bp_desc = '';
if ($id != 0) {
    $sql = "SELECT name, description FROM bp_asset WHERE id=?";
    $data = $conn->getRow($sql, array($id));
    if ($data === false) {
        die($conn->ErrorMsg());
    }
    list($bp_name, $bp_desc) = $data;
    
    $sql = "SELECT type_name FROM bp_asset_member_type";
    $bp_types = $conn->getAll($sql);
    if ($bp_types === false) {
        die($conn->ErrorMsg());
    }
    
    $users = get_users();
    if ($users === false) {
        die($conn->ErrorMsg());
    }
}
?>
<html>
<head>
  <title><?_("Business Processes")?></title>
  <script src="../js/prototype.js" type="text/javascript"></script>
  <link rel="stylesheet" href="../style/style.css"/>
<?= $xajax->printJavascript('', XAJAX_JS); ?>
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
<? if ($id == 0) { ?>
    <h2><?=_("New Asset wizard")?></h2>
<? } else { ?>
    <h2><?=_("Edit Asset")?>: <?=$data['name']?></h2>
<? } ?>

<form id="bp_form">
<table width="100%"><tr><td style="border-width: 0px">
<table width="60%" style="border-width: 0px">
<tr>
    <th><?=_("Name")?></th>
    <td style="text-align: left; border-width: 0px"><input type="text" size="40" name="bp_name" value="<?=$bp_name?>"></td>
</tr>
<tr>
    <th><?=_("Description")?></th>
    <td style="text-align: left; border-width: 0px">
        <textarea NAME="bp_desc" COLS="40" ROWS=5 WRAP=HARD><?=$bp_desc?></textarea>
    </td>
</tr>
</table>
<br>
<? if ($id == 0) { ?>
    <center>
    <input type="button" value="<?=_("Cancel")?>"
           onClick="javascript: history.go(-1);">&nbsp;
    <input type="button" value="<?=_("Continue")?>"
           onClick="javascript: xajax_edit_asset(xajax.getFormValues('bp_form'))">
    </center>
<?
} else {
?>

<table width="60%"><tr><th><?=_("Responsibles")?></th></tr></table>
<div id="responsibles" class="contents">
<!-- Filled by draw_responsibles() -->
</div>
<script>xajax_draw_responsibles(false)</script>
<br>
<div class="row" style="text-align: center; width: 60%">
    <select name="bp_new_responsible">
    <? foreach ($users as $u) { ?>
        <option value="<?=$u[0]?>"><?=$u[1]?> (<?=$u[0]?>)</option>
    <? } ?>
    </select>
    <input type="button" onClick="javascript: xajax_draw_responsibles(xajax.getFormValues('bp_form', true, 'bp_new_responsible'))" value="Add">
</div>
<br>

<table width="60%"><tr><th><?=_("Members")?></th></tr></table>
<div id="members" class="contents">
<!-- Filled by draw_members() -->
</div>
<script>xajax_draw_members(false)</script>
<br/>
<div class="row" style="width: 80%">
<table width="100%" align="center">
<tr><th colspan="2"><?=_("Insert New Member")?></th></tr>
<tr>
    <td><?=_("Type")?></td>
    <td style="text-align: left">
        <select name="member_type"
                onChange="javascript: $(members_select).innerHTML = '<b><i><?=_("Loading...")?></i></b>'; xajax_draw_members_select(xajax.getFormValues('bp_form', true, 'member_type'));">
            <option></option>
        <? foreach ($bp_types as $i => $type) { ?>
            <option value="<?=$type[0]?>"><?=$type[0]?></option>
        <? } ?>
        </select>
    </td>
</tr><tr>
    <td><?=_("Name")?></td>
    <td id="members_select" style="text-align: left"><?=_("Please select a type")?></td>
</tr><tr>
    <td colspan="2">
    <input type="button" onClick="javascript: xajax_draw_members(xajax.getFormValues('bp_form', true, 'member_'))" value="Add">
    </td>
</tr>
</table>
</div>
</td><td>
<input type="button" value="<?=_("Continue")?>-&gt;"
       onClick="javascript: xajax_edit_asset(xajax.getFormValues('bp_form'))">
</tr></table>
<? } ?>
</form>
</body>
</html>
