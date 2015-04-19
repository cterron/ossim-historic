<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationRRDConfig");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("New RRD Profile"); ?> </h1>

<?php

    require_once 'classes/Security.inc';
    
    $profile = REQUEST('profile');

    ossim_valid($profile, OSS_ALPHA, OSS_SPACE, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("Profile"));

    if (ossim_error()) {
        die(ossim_error());
    }



    require_once ('classes/RRD_config.inc');
    require_once ('ossim_db.inc');

    $db = new ossim_db();
    $conn = $db->connect();

    if ($rrd_list = RRD_Config::get_list($conn,  "WHERE profile = 'Default'"))
    {
        foreach ($rrd_list as $rrd)
        {
            $attrib = $rrd->get_rrd_attrib();

            if (POST("$attrib#rrd_attrib"))
            {
                if (POST("$attrib#enable") == "on")
                    $enable = 1;
                else
                    $enable = 0;

                ossim_valid(POST("$attrib#rrd_attrib"), OSS_ALPHA, OSS_SPACE, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("$attrib#rrd_attrib"));
                ossim_valid(POST("$attrib#threshold"), OSS_ALPHA, OSS_SPACE, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("$attrib#threshold"));
                ossim_valid(POST("$attrib#priority"), OSS_ALPHA, OSS_SPACE, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("$attrib#priority"));
                ossim_valid(POST("$attrib#alpha"), OSS_ALPHA, OSS_SPACE, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("$attrib#alpha"));
                ossim_valid(POST("$attrib#beta"), OSS_ALPHA, OSS_SPACE, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("$attrib#beta"));
                ossim_valid(POST("$attrib#persistence"), OSS_ALPHA, OSS_SPACE, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("$attrib#persistence"));

                if (ossim_error()) {
                    die(ossim_error());
                }

                RRD_Config::insert ($conn,
                                    $profile,
                                    POST("$attrib#rrd_attrib"),
                                    POST("$attrib#threshold"),
                                    POST("$attrib#priority"),
                                    POST("$attrib#alpha"),
                                    POST("$attrib#beta"),
                                    POST("$attrib#persistence"),
                                    $enable);
            }
        }
    }

    $db->close($conn);

?>
    <p> <?php echo gettext("RRD Config succesfully inserted"); ?> </p>
    <p><a href="rrd_conf.php"> <?php echo gettext("Back"); ?> </a></p>

</body>
</html>

