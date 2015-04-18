<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationRiskmeter");
?>

<html>
<head>
  <title> OSSIM </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
                                                                                
<body>

  <h1>Configuration</h1>

<?php

    /* check params */
    if ((!$_POST["threshold"]) ||
        (!$_POST["graph_threshold"]) || (!$_POST["bar_length_left"]) ||
        (!$_POST["bar_length_right"]))
    {
?>
                                                                                
  <p align="center">Please, complete all the fields</p>
  <?php exit();?>
                                                                                
<?php
    } else {
        $recovery         = mysql_escape_string($_POST["recovery"]);
        $threshold        = mysql_escape_string($_POST["threshold"]);
        $graph_threshold  = mysql_escape_string($_POST["graph_threshold"]);
        $bar_length_left  = mysql_escape_string($_POST["bar_length_left"]);
        $bar_length_right = mysql_escape_string($_POST["bar_length_right"]);

        require_once 'ossim_db.inc';
        require_once 'classes/Conf.inc';
        $db = new ossim_db();
        $conn = $db->connect();

        Conf::update($conn, $recovery, $threshold, $graph_threshold,
                            $bar_length_left, $bar_length_right);

        $db->close($conn);
    }

?>
    <p align="center">Done.</p>
    
</body>
</html>

