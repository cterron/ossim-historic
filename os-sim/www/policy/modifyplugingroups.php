<?php
require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
require_once 'ossim_db.inc';
require_once 'classes/Plugin.inc';

Session::logcheck("MenuPolicy", "PolicyPluginGroups");

$db = new ossim_db();
$conn = $db->connect();

$plugin_list = Plugin::get_list($conn, "ORDER BY name");

/*
 * Sample valid $str values:
 *      '0' => ALL SIDs
 *      '1' => only SID 1
 *      '1,2' => SIDs 1 and 2
 *      '1-4' => All SIDs between 1 and 4 (both inclusive)
 *      '1,3-5' => SID 1 and range 3 to 5
 *      '3-5,46,47,110-170' => Valid too
 */
function validate_sids_str($str)
{
    $values = preg_split('/(\s*,\s*)/', $str);
    $ret = $m = array();
    foreach ($values as $v) {
        if (preg_match('/^([1-9][0-9]*)-([1-9][0-9]*)$/', $v, $m)) {
            list($start, $end) = array($m[1], $m[2]);
            if ($start >= $end) {
                return array(false, _("Invalid range: '$v'"));
            }
            $ret[] = $v;
        } elseif (preg_match('/^[0-9]+$/', $v, $m)) {
            $ret[] = $v;
        } else {
            return array(false, _("Invalid sid: '$str'"));
        }
    }
    // $str = '0,1,2'
    if (count($ret) > 1 && in_array(0, $ret)) {
        return array(false, _("'0' should be alone and means ALL sids, sid: '$str' not valid"));
    }
    // $str = '';
    if (!count($ret)) {
        return array(false, _("Sid can not be empty. Specify '0' if you want ALL sids"));
    }
    return array(true, implode(',', $ret));
}
/*
 * Validates the POST data: name, description, plugins and SIDs
 * 
 * @return Processed array($name, $description, array(plug_id => sid string))
 */
function validate_post_params($name, $descr, $sids)
{
    $vals = array(
        'name'  => array(OSS_INPUT, 'illegal:'._("Name")),
        'descr' => array(OSS_TEXT,  'illegal:'._("Description")),
    );
    ossim_valid($name, $vals['name']);
    ossim_valid($descr, $vals['descr']);
    $plugins = array();
    $sids = is_array($sids) ? $sids : array(); 
    foreach ($sids as $plugin => $sids_str) {
        if ($sids_str !== '') {
            list($valid, $data) = validate_sids_str($sids_str);
            if (!$valid) {
                ossim_set_error(_("Error for plugin ").$plugin.': '.$data);
                break;
            }
            $plugins[$plugin] = $sids_str;
        }
    }
    if (!count($plugins)) {
        ossim_set_error(_("No plugins or SIDs selected"));
    }
    if (ossim_error()) {
        die(ossim_error());
    }
    return array($name, $descr, $plugins);
}

if (isset($_GET['interface']) && isset($_GET['method'])) {
    list($valid, $data) = validate_sids_str($_GET['sids_str']);
    if (!$valid) {
        echo $data;
    }
    exit;
}

$db = new ossim_db();
$conn = $db->connect();

//
// Insert new
//
if (GET('action') == 'new') {

    list($name, $descr, $plugins) = validate_post_params(POST('name'),
                                                         POST('descr'),
                                                         POST('sids'));
    // Insert section
    //
    $group_id = $conn->GenID('plugin_group_descr_seq');
    $conn->StartTrans();
    $sql = "INSERT INTO plugin_group_descr" .
           "(group_id, name, descr) " .
           "VALUES (?, ?, ?)";
    $conn->Execute($sql, array($group_id, $name, $descr));
    
    $sql = "INSERT INTO plugin_group " .
           "(group_id, plugin_id, plugin_sid) " .
           "VALUES (?, ?, ?)";
    foreach ($plugins as $plugin => $sids_str) {
        $conn->Execute($sql, array($group_id, $plugin, $sids_str));
    }
    $conn->CompleteTrans();
    if ($conn->HasFailedTrans()) {
        die($conn->ErrorMsg());
    }
//
// Edit group
//
} elseif (GET('action') == 'edit') {
    list($name, $descr, $plugins) = validate_post_params(POST('name'),
                                                         POST('descr'),
                                                         POST('sids'));
    $group_id = GET('id');
    ossim_valid($group_id, OSS_DIGIT, 'illegal:ID');
    if (ossim_error()) {
        die(ossim_error());
    }
    $conn->StartTrans();
    $sql = "UPDATE plugin_group_descr
            SET name=?, descr=?
            WHERE group_id=?";
    $conn->Execute($sql, array($name, $descr, $group_id));
    $conn->Execute("DELETE FROM plugin_group WHERE group_id=$group_id");
    $sql = "INSERT INTO plugin_group " .
           "(group_id, plugin_id, plugin_sid) " .
           "VALUES (?, ?, ?)";
    foreach ($plugins as $plugin => $sids_str) {
        $conn->Execute($sql, array($group_id, $plugin, $sids_str));
    }
    $conn->CompleteTrans();
    if ($conn->HasFailedTrans()) {
        die($conn->ErrorMsg());
    }
    
//
// Delete group
//
} elseif (GET('action') == 'delete') {
    $group_id = GET('id');
    ossim_valid($group_id, OSS_DIGIT, 'illegal:ID');
    if (ossim_error()) {
        die(ossim_error());
    }
    $conn->StartTrans();
    $conn->Execute("DELETE FROM plugin_group WHERE group_id=$group_id");
    $conn->Execute("DELETE FROM policy_plugin_group_reference WHERE group_id=$group_id");
    $conn->Execute("DELETE FROM plugin_group_descr WHERE group_id=$group_id");
    $conn->CompleteTrans();
    if ($conn->HasFailedTrans()) {
        die($conn->ErrorMsg());
    }
}
header('Location: plugingroups.php');
?>
