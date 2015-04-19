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

  <h1> <?php echo gettext("Delete Incident type"); ?> </h1>

<?php
    
    require_once ('classes/Security.inc');
    
    $inctype_id = GET('inctype_id');

    ossim_valid($inctype_id, OSS_ALPHA, 'illegal:'._("Incident ID"));

    if (ossim_error()) {
        die(ossim_error());
    }

    if ( !Session::am_i_admin() )
    {
      require_once("ossim_error.inc");
      $error = new OssimError();
      $error->display("ONLY_ADMIN");
    }


if (!GET('confirm')) {
?>
    <p> <?php echo gettext("Are you sure"); ?> </p>
    <p><a
      href="<?php echo $_SERVER["PHP_SELF"].
        "?inctype_id=$inctype_id&confirm=yes"; ?>"> 
	<?php echo gettext("Yes"); ?> </a>
        &nbsp;&nbsp;&nbsp;<a href="incident_type.php"> <?php echo gettext("No"); ?> </a>
    </p>
<?php
    exit();
}



    require_once 'ossim_db.inc';
    require_once ("classes/Incident_type.inc");
    $db = new ossim_db();
    $conn = $db->connect();
    
    Incident_type::delete($conn, $inctype_id);
    $db->close($conn);

?>

    <p> <?php echo gettext("Action type deleted"); ?> </p>
    <p><a href="incidenttype.php"> <?php echo gettext("Back"); ?> </a></p>
    <?php exit(); ?>

</body>
</html>

