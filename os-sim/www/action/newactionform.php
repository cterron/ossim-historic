<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyActions");

require_once ('classes/Action.inc');
require_once ('classes/Action_type.inc');
require_once ('ossim_db.inc');

$action_id = REQUEST('id');
$action_type = REQUEST('action_type');
$descr = REQUEST('descr');
$email_from = REQUEST('email_from');
$email_to = REQUEST('email_to');
$email_subject = REQUEST('email_subject');
$email_message = REQUEST('email_message');
$exec_command = REQUEST('exec_command');


ossim_valid($action_id, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("Action id"));
ossim_valid($action_type, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("Action type"));
ossim_valid($descr, OSS_ALPHA, OSS_PUNC, OSS_SCORE, OSS_AT, OSS_NULLABLE, 'illegal:'._("Description"));
ossim_valid($email_from, OSS_MAIL_ADDR, OSS_NULLABLE, 'illegal:'._("Email from"));
foreach (split(',', $email_to) as $to_mail) {
    $to_mail = trim($to_mail);
    ossim_valid($to_mail, OSS_MAIL_ADDR, OSS_NULLABLE, 'illegal:'._("Email to"));
}
ossim_valid($email_subject, OSS_ALPHA, OSS_PUNC, OSS_SCORE, OSS_AT, "><", OSS_NULLABLE, 'illegal:'._("Email subject"));
ossim_valid($email_message, OSS_ALPHA, OSS_PUNC, OSS_SCORE, OSS_AT, "><", OSS_NULLABLE, OSS_NL, 'illegal:'._("Email message"));
ossim_valid($exec_command, OSS_ALPHA, OSS_PUNC, OSS_SCORE, OSS_AT, "><", OSS_NULLABLE, 'illegal:'._("Exec command"));


if (ossim_error()) {
    die(ossim_error());
}

$db = new ossim_db();
$conn = $db->connect();

if (REQUEST('insert_action')) {

    if ($action_type == "email") {

        if ( (REQUEST('descr')) and
             (REQUEST('email_from')) and
             (REQUEST('email_to')) and
             (REQUEST('email_subject')) and
             (REQUEST('email_message')) )
        {
            Action::insertEmail($conn, $action_type, $descr, $email_from, $email_to, $email_subject, $email_message);
            message_ok();
            exit;
        } else {
            require_once("ossim_error.inc");
            $error = new OssimNotice();
            $error->display("FORM_NOFILL");
        }

    } elseif ($action_type == "exec") {

        if ( (REQUEST('descr')) and
             (REQUEST('exec_command')) )
        {
            Action::insertExec($conn, $action_type, $descr, $exec_command);
            message_ok();
            exit();
        } else {
            require_once("ossim_error.inc");
            $error = new OssimNotice();
            $error->display("FORM_NOFILL");
        }
    }
}

function message_ok() {
    echo '<p align="center">';
    echo gettext ("Action inserted successfully");
    echo '<br/><a href="action.php">';
    echo gettext("Back");
    echo '</a></p>';
}

function email_form()
{
?>
    <tr><td colspan="2">&nbsp;</td></tr>
    <tr>
      <td> <?php echo gettext("From:"); ?> </td>
      <td><input name="email_from" type="text" size="60" /></td>
    </tr>
    <tr>
      <td> <?php echo gettext("To:"); ?> </td>
      <td><input name="email_to" type="text" size="60" /></td>
    </tr>
    <tr>
      <td> <?php echo gettext("Subject:"); ?> </td>
      <td><input name="email_subject" type="text" size="60" /></td>
    </tr>
    <tr>
      <td> <?php echo gettext("Message:"); ?> </td>
      <td><textarea name="email_message" type="text" rows="10" cols="80" WRAP=HARD></textarea></td>
    </tr>
<?php
}


function exec_form()
{
?>
    <tr><td colspan="2">&nbsp;</td></tr>
    <tr>
      <td> <?php echo gettext("Command:"); ?> </td>
      <td><input name="exec_command" type="text" /></td>
    </tr>
<?php
}


function submit()
{
?>
    <tr><td colspan="2">
      <input type="submit" name="insert_action" value="OK"></td>
    </tr>
<?php
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

<h1> <?php echo gettext("New action"); ?> </h1>

<table align="center" width="50%">
<tr>
<td colspan="2" style="text-align: left">
<?php 
echo gettext("You can use the following keywords within any field which will be get substituted by it's matching value upon action execution") . "."; 
?>
<table width="80%" align="center" style="border-width: 0px"><tr>
<td style="text-align: left" valign="top">
<ul>
<li> DATE
<li> PLUGIN_ID
<li> PLUGIN_SID
<li> RISK
<li> PRIORITY
<li> RELIABILITY
<li> SRC_IP
<li> DST_IP
<li> SRC_PORT
<li> DST_PORT
<li> PROTOCOL
<li> SENSOR
<li> BACKLOG_ID
<li> EVENT_ID
</ul>
<td style="text-align: left" valign="top">
<ul>
<li> PLUGIN_NAME
<li> SID_NAME
<li> USERNAME
<li> PASSWORD
<li> FILENAME
<li> USERDATA1
<li> USERDATA2
<li> USERDATA3
<li> USERDATA4
<li> USERDATA5
<li> USERDATA6
<li> USERDATA7
<li> USERDATA8
<li> USERDATA9
</ul>
</td></tr></table>
</td></tr>
<form name="new_action" method="POST">
  <tr>
    <th> <?php echo gettext("Description"); ?> </th>
    <td>
      <textarea name="descr"><?php echo $descr ?></textarea>
    </td>
  </tr>
  <tr>
    <th>Type</th>
    <td>
      <select name="action_type"
        onChange="document.forms['new_action'].submit()">
        <option value=""> -- <?php echo gettext("Select an action type"); ?> -- </option>
<?php
    if (is_array($action_type_list = Action_type::get_list($conn))) {
        foreach ($action_type_list as $action_type_aux) {
?>
        <option
            value="<?php echo $action_type_aux->get_type() ?>"
            <?php
                if ($action_type == $action_type_aux->get_type()) 
                    echo " SELECTED ";
            ?>>
            <?php echo $action_type_aux->get_descr(); ?>
        </option>
<?php
        }
    }
?>
      </select>
    </td>
  </tr>

<?php
    /* type of action */
    if ($action_type == "email") {
        email_form();
        submit();
    } elseif ($action_type == "exec") {
        exec_form();
        submit();
    }
?>

  </form>
</table>

<?php
$db->close($conn);
?>

</body>
</html>

