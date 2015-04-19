<?php
require_once 'classes/Session.inc';
require_once 'classes/Security.inc';
require_once 'classes/Upgrade.inc';

//Session::logcheck("MainMenu", "Index", "session/login.php");

if (!Session::am_i_admin()) {
    die(_("Not enough perms"));
}

set_time_limit(0);
ignore_user_abort(true);
ob_implicit_flush(true);

$version = GET('version');
$type = GET('type');
$force = GET('force');

ossim_valid($version, OSS_DIGIT, OSS_LETTER, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("version"));
ossim_valid($type, OSS_ALPHA, OSS_SCORE, OSS_NULLABLE, 'illegal:'._("type"));
ossim_valid($force, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("force"));

if (ossim_error()) {
    die(ossim_error());
}

$upgrade = new Upgrade();

if (GET('submit')) {
    $ok = $upgrade->needs_upgrade();
    if (!$ok) die(_("No upgrades needed"));
    if (ossim_error()) die(_("Not clean installation detected. Refusing to apply upgrades, please do it manually"));
    
    $upgrade->apply_needed();
    echo '<a href="'.$_SERVER['PHP_SELF'].'">'._("Continue").'</a>'; exit;
    exit;
}

// Force a certain upgrade
if (GET('version') && GET('type') && GET('force')) {
    $upgrades = $upgrade->get_all();
    if (!isset($upgrades[$version])) {
        die(_("Error: no valid version upgrade"));
    }
    switch ($type) {
        case 'php_pre':
            $file = $upgrades[$version]['php']['file'];
            $upgrade->create_php_upgrade_object($file, $version);
            // XXX Move that to the main class
            echo "<pre>"._("Starting PHP PRE script")."...\n";
            $upgrade->php->start_upgrade();
            echo "\n"._("PHP PRE script ended")."</pre>";
            $upgrade->destroy_php_upgrade_object();
            break;
        case 'php_post':
            $file = $upgrades[$version]['php']['file'];
            $upgrade->create_php_upgrade_object($file, $version);
            echo "<pre>"._("Starting PHP POST script")."...\n";
            $upgrade->php->end_upgrade();
            echo "\n"._("PHP POST script ended")."</pre>";
            $upgrade->destroy_php_upgrade_object();
            break;
        case 'sql':
            $file = $upgrades[$version]['sql']['file'];
            $upgrade->execute_sql($file, true);
            break;
    }
    if (ossim_error()) die(ossim_error());
    
    echo '<a href="'.$_SERVER['PHP_SELF'].'">'._("Continue").'</a>'; exit;
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
<table width="50%" align="center">
<tr>
    <th>Detected Ossim Version:</th><td><?=$upgrade->ossim_current_version?>&nbsp;</td>
</tr>
<tr>
    <th>Detected Schema Version:</th><td><?=$upgrade->ossim_schema_version?>&nbsp;</td>
</tr>
<tr>
    <th>Detected Database Type:</th><td><?=$upgrade->ossim_dbtype?>&nbsp;</td>
</tr>
</table>
<br/>
<?

function print_upgrade_link($file, $type, $label, $version, $required)
{
    echo "$file&nbsp; (";
    if (!$required) {
        $confirm = _('This will force only this upgrade and ' .
                     'may cause unexpected results. Use the \\\'Apply Changes\\\' ' .
                     'button instead.\n\nContinue anyway?'); 
        echo "<a href=\"?version=$version&type=$type&force=1\"
                 onClick=\"return confirm('$confirm')\">$label</a> )";
    } else {
        echo "$label)";
    }
}

$list[0]['name'] = _("Required upgrades");
$list[0]['upgrades'] = $upgrade->get_needed();
$list[0]['required'] = true;

// this method search for errors and sets them via ossim_set_error()
$upgrade->needs_upgrade(); 
if (ossim_error()) echo ossim_error();

$list[1]['name'] = _("All upgrades");
$list[1]['upgrades'] = $upgrade->get_all();
$list[1]['required'] = false;

foreach ($list as $k => $v) {
    
?>
    <h3><?=$v['name']?></h3>
    <? if (!count($v['upgrades'])) { ?>
        <br/><i><center><?=_("No upgrades")?></center></i>
    <? continue; }  ?>
    <form>
    <table align="center" width="85%" border=1>
        <tr><th>Version</th><th>Required</th></tr>
        <? foreach ($v['upgrades'] as $version => $actions) { ?>
            <tr>
                <td><?=$version?></td>
                <td style="text-align: left;">
                <?
                $pos = 0;
                $php = isset($actions['php']['file']) ? $actions['php']['file'] : '';
                $sql = isset($actions['sql']['file']) ? $actions['sql']['file'] : '';
                $error = isset($actions['error']['file']) ? $actions['error']['file'] : '';
                if ($error) {
                    echo "<font color=red>$error</font>";
                    continue;
                }
                if ($php && ++$pos) {
                    echo "<br/>{$pos}º ";
                    print_upgrade_link($php, 'php_pre', 'PHP script: PRE', $version, $v['required']);
                }
                if ($sql && ++$pos) {
                    echo "<br/>{$pos}º ";
                    print_upgrade_link($sql, 'sql', 'SQL schema update', $version, $v['required']);
                }
                if ($php && ++$pos) {
                    echo "<br/>{$pos}º ";
                    print_upgrade_link($php, 'php_post', 'PHP script: POST', $version, $v['required']);
                }
                echo "<br/>&nbsp;";
                ?>
            </tr>
        <? } ?>
    </table>
    <br/>
    <? if ($v['required']) { ?>
    <center>
        <input type="submit" name="submit"
            value="<?=_("Apply Changes")?>"
            onClick="return confirm('<?=_("IMPORTANT: Please make sure you have made a backup of the database before continue")?>')"
            >
    </center>
    <? } ?>
    </form><br>
<? } ?>
