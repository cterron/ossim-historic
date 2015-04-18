<?php

    require_once ('ossim_db.inc');
    require_once ('classes/Host_qualification.inc');
    $db = new ossim_db();
    $conn = $db->connect();
    $ip = mysql_escape_string($_GET["ip"]);

    Host_qualification::delete($conn, $ip);
?>    
    <p align="center">Reset completed</p>

<?php
    $db->close($conn);
?>


