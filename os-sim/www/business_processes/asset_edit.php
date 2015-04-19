<?php
require_once 'classes/Session.inc';
require_once 'classes/Security.inc';
require_once 'classes/Xajax.inc';
require_once 'classes/Util.inc';

$db = new ossim_db();
$conn = $db->connect();

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
            $sql = "INSERT INTO bp_asset_member (asset_id, member, member_type) " .
                   "VALUES (?, ?, ?)";
            $params = array($id, $form_data["member_name"], $form_data["member_type"]);
            if (!$conn->Execute($sql, $params)) {
                $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
                return $resp;
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
    global $conn, $id;
    $resp = new xajaxResponse();
    ossim_valid($form_data['bp_name'], OSS_INPUT, 'illegal:'._("Name"));
    ossim_valid($form_data['bp_desc'], OSS_TEXT, 'illegal:'._("Description"));
    if (ossim_error()) {
        $resp->AddAssign("form_errors", "innerHTML", ossim_error());
    } else {
        // New record
        if ($id == 0) {
            $sql = "INSERT INTO bp_asset (id, name, description) VALUES (?, ?, ?)";
            $id = $conn->GenID('bp_seq');
            $params = array($id, $form_data['bp_name'], $form_data['bp_desc']);
            if (!$conn->Execute($sql, $params)) {
                $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
            } else {
                $resp->addRedirect($_SERVER['PHP_SELF']."?id=$id");
            }
        // Continue button, reflect possible changes in name or description
        } else {
            $sql = "UPDATE bp_asset SET name=?, description=? WHERE id=?";
            $params = array($form_data['bp_name'], $form_data['bp_desc'], $id);
            if (!$conn->Execute($sql, $params)) {
                $resp->AddAssign("form_errors", "innerHTML", $conn->ErrorMsg());
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
<div class="row" style="width: 40%">
<table width="50%" align="right">
<tr><th colspan="2"><?=_("Insert New Member")?></th></tr>
<tr>
    <td><?=_("Type")?></td>
    <td style="text-align: left">
        <select name="member_type">
        <? foreach ($bp_types as $i => $type) { ?>
            <option><?=$type[0]?></option>
        <? } ?>
        </select>
    </td>
</tr><tr>
    <td><?=_("Name")?></td>
    <td><input type="text" name="member_name"></td>
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