<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

<?php

    require_once 'ossim_sql.inc';
    require_once 'classes/Security.inc';

    $id = GET('id');    
    
    ossim_valid($id, OSS_ALPHA, OSS_SPACE, OSS_SCORE, 'illegal:'._("Response ID"));

    if (ossim_error()) {
        die(ossim_error());
    }

    require_once ("ossim_db.inc");
    require_once ("classes/Response.inc");

    $db = new ossim_db();
    $conn = $db->connect();

    Response::delete($conn, $id);

    $db->close($conn);

    echo '<p align="center">';
    echo gettext("Response deleted");
    echo '<br/><a href="response.php">';
    echo gettext("Back");
    echo '</a></p>'

?>

</body>
</html>

