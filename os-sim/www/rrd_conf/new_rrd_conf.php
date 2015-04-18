<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>New RRD Config</h1>

<?php

    /* check params */
    if (!mysql_escape_string($_POST["ip"]))
    {
        echo "<p align=\"center\">Please, complete all the fields</p>";
        exit();
    }

    require_once ('classes/RRD_config.inc');
    require_once ('ossim_db.inc');

    $db = new ossim_db();
    $conn = $db->connect();

    if ($rrd_list = RRD_Config::get_list($conn,  "WHERE ip = 0"))
    {
        foreach ($rrd_list as $rrd) 
        {
            $attrib = $rrd->get_rrd_attrib();
        
            if (isset($_POST["$attrib#rrd_attrib"]))
            {
                RRD_Config::insert ($conn, 
                                    $_POST["ip"], 
                                    $_POST["$attrib#rrd_attrib"], 
                                    $_POST["$attrib#threshold"], 
                                    $_POST["$attrib#priority"], 
                                    $_POST["$attrib#alpha"], 
                                    $_POST["$attrib#beta"], 
                                    $_POST["$attrib#persistence"]);
            }
        }
    }

    $db->close($conn);

?>
    <p>RRD Config succesfully inserted</p>
    <p><a href="rrd_conf.php">Back</a></p>

</body>
</html>

