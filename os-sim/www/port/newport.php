<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyPorts");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("New port"); ?> </h1>

<?php

$name = POST('name');
$nports = POST('nports');
$descr = POST('descr');

ossim_valid($name, OSS_ALPHA, OSS_SPACE, OSS_PUNC, 'illegal:'._("name"));
ossim_valid($nports, OSS_DIGIT, 'illegal:'._("nportse"));
ossim_valid($descr, OSS_ALPHA, OSS_SPACE, OSS_PUNC, OSS_AT, 'illegal:'._("Descrition"));

if (ossim_error()) {
           die(ossim_error());
}

/* check OK, insert into BD */
if(POST('insert')) {

    require_once 'ossim_db.inc';
    require_once 'classes/Port_group.inc';
    $db = new ossim_db();
    $conn = $db->connect();

    for ($i = 1; $i <= $nports; $i++) {
        $mboxname = "mbox" . $i;
        if (POST("$mboxname")) {
            ossim_valid(POST("$mboxname"), OSS_ALPHA, OSS_SCORE, OSS_SPACE, 'illegal'._("Port"));
            if (ossim_error()) {
                die(ossim_error());
            }
            $port_aux = POST("$mboxname");
            if (!empty($port_aux))
                $port_list[] = POST("$mboxname");
        }
    }
   
    Port_group::insert ($conn, $name, $port_list, $descr);

    $db->close($conn);
}
?>
    <p> <?php echo gettext("Port succesfully inserted"); ?> </p>
    <p><a href="port.php">
    <?php echo gettext("Back"); ?> </a></p>

</body>
</html>

