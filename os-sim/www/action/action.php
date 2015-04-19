<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyActions");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("Actions"); ?> </h1>

<?php

    require_once ('ossim_db.inc');
    require_once ('classes/Action.inc');

    $db = new ossim_db();
    $conn = $db->connect();
    if (is_array($action_list = Action::get_list($conn))) {
?>
    <table align="center">
      <tr>
        <th> <?php echo gettext("Description"); ?>&nbsp;</th>
        <th> <?php echo gettext("Action"); ?>&nbsp;</th>
      </tr>
<?php
        foreach ($action_list as $action) {
?>
      <tr>
        <?php $id = $action->get_id(); ?>
        <td><?php echo ($action = $action->get_descr()); ?></td>
        <td>
          [<a href="modifyactionform.php?id=<?php echo $id ?>"><?php echo gettext("Modify"); ?></a>]
          [<a href="deleteaction.php?id=<?php echo $id ?>"><?php echo gettext("Delete"); ?></a>]
        </td>
      </tr>
<?php
        }
?>
      <tr>
        <td colspan="2">
        <a href="newactionform.php"><?php echo gettext("Add new action") ?></a>
        </td>
      </tr>
    </table>
<?php
    } else {
?>
        <p align=\"center\">
        <?php echo gettext("There are no defined actions") ?>
        <br/>
        <a href="newactionform.php"><?php echo gettext("Add new action") ?></a>
        </p>
<?php
    }
    $db->close($conn);

?>


</body>
</html>

