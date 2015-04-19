<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuIncidents", "IncidentsTypes");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Incidents types"); ?> </h1>

<?php

    require_once 'ossim_db.inc';
    require_once 'classes/Incident_type.inc';

    $db = new ossim_db();
    $conn = $db->connect();

?>
    <!-- main table -->
    <table align="center">
<?php
    if ($inctype_list = Incident_type::get_list($conn, "")) {
?>
    <tr>
        <th><?php echo gettext("Id"); ?></th>
        <th><?php echo gettext("Description"); ?></th>
        <th><?php echo gettext("Actions"); ?></th>
    </tr>    
    
<?php 
    foreach ($inctype_list as $inctype)
    {
?>
        <tr>
            <td><?php echo $inctype->get_id(); ?></td>
            <td>
            <?php 
              if ( "" == $inctype->get_descr()) {
                  echo " -- ";
              } else {
                  echo $inctype->get_descr();
              }
            ?>
            </td>
            <td>
            <?php
            
            if (!("Generic" == $inctype->get_id())){
            echo "[<a
            href=\"modifyincidenttypeform.php?id=".$inctype->get_id()."\"> ".gettext("Modify")." </a>] [
            <a href=\"deleteincidenttype.php?inctype_id=".$inctype->get_id()."\"> ".gettext("Delete")." 
            </a>]";
            } else {
                echo " -- ";
            }
            ?>
            </td>
        </tr>
<?php
    }
?>



<?php
    } else {
       echo "error";
    }
?>
    <tr>
    <td colspan="3" align="center"><a href="newincidenttypeform.php"><?php echo gettext("Add new type"); ?></a><td>
    </tr>
    </table>

</body>
</html>
<?php
    $db->close($conn);
?> 
