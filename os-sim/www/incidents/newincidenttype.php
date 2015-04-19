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

  <h1> <?php echo gettext("New Incident type"); ?> </h1>

<?php
    require_once 'classes/Security.inc';
    
    $inctype_id = POST('id');
    $inctype_descr = POST('descr');
    
    ossim_valid($inctype_descr, OSS_ALPHA, OSS_SPACE, OSS_PUNC, OSS_AT, 'illegal:'._("Description"));
    ossim_valid($inctype_id, OSS_ALPHA, OSS_SPACE, OSS_PUNC, 'illegal:'._("id"));
    
    if (ossim_error()) {
        die(ossim_error());
    }

    if (POST('insert')) {

        require_once ('ossim_db.inc');
        require_once ('classes/Incident_type.inc');

        $db = new ossim_db();
        $conn = $db->connect();
        
        require_once("classes/Incident_type.inc");
        Incident_type::insert ($conn, $inctype_id, $inctype_descr);

        $db->close($conn);
?>
    <p> <?php echo gettext("New incident type  succesfully inserted"); ?> </p>
    <p><a href="incidenttype.php"> <?php echo gettext("Back"); ?> </a></p>
<?php
    }
?>


</body>
</html>

