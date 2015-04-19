<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

<?php

    require_once 'ossim_db.inc';
    require_once 'classes/Action.inc';
    require_once 'classes/Security.inc';

    $action_id = GET('id');
    
    ossim_valid($action_id, OSS_DIGIT, 'ilegal:'._("Action id"));

    if (ossim_error()) {
        die(ossim_error());
    }
                            
    $db = new ossim_db();
    $conn = $db->connect();

    Action::delete($conn, $action_id);

    $db->close($conn);

    echo '<p align="center">';
    echo gettext("Action deleted");
    echo '<br/><a href="action.php">';
    echo gettext("Back");
    echo '</a></p>'

?>

</body>
</html>
