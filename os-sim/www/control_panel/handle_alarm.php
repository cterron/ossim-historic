<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>OSSIM Framework</h1>

<?php
require_once 'ossim_db.inc';
require_once 'classes/Backlog.inc';

?>

<?php

$db = new ossim_db();
$conn = $db->connect();

while (list($utime) = each($_GET)) {
    Backlog::delete($conn, $utime);
}

$db->close($conn);

?>
    <p>Successfully Acknowledged</p>
    <p><a href="alarm_console.php">Back</a></p>

</body>
</html>

