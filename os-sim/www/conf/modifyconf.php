<html>
<head>
  <title> OSSIM </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
                                                                                
<body>

  <h1>OSSIM Framework</h1>
  <h2>Configuration</h2>

<?php

    /* check params */
    if ((!$_POST["recovery"]) || (!$_POST["threshold"]) ||
        (!$_POST["graph_threshold"]) || (!$_POST["bar_length_left"]) ||
        (!$_POST["bar_length_right"]))
    {
?>
                                                                                
  <p align="center">Please, complete all the fields</p>
  <?php exit();?>
                                                                                
<?php
    } else {
        $recovery         = $_POST["recovery"];
        $threshold        = $_POST["threshold"];
        $graph_threshold  = $_POST["graph_threshold"];
        $bar_length_left  = $_POST["bar_length_left"];
        $bar_length_right = $_POST["bar_length_right"];

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

