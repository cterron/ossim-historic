<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationUsers");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Modify User"); ?> </h1>

<?php

    require_once ("classes/Security.inc");

    $user  = POST('user');
    $pass1 = POST('pass1');
    $pass2 = POST('pass2');
    $name  = POST('name');
    $email = POST('email');
    $nnets = POST('nnets');
    $nsensors = POST('nsensors');
    $company = POST('company');
    $department = POST('department');

    ossim_valid($user, OSS_USER, 'illegal:'._("User name"));
    ossim_valid($name, OSS_ALPHA, OSS_PUNC, OSS_AT, OSS_SPACE, 'illegal:'._("Name"));
    ossim_valid($email, OSS_NULLABLE, OSS_MAIL_ADDR, 'illegal:'._("e-mail"));
    ossim_valid($nnets, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("nnets"));
    ossim_valid($nsensors, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("nsensors"));
    ossim_valid($company, OSS_ALPHA, OSS_PUNC, OSS_AT, OSS_NULLABLE, 'illegal:'._("Company"));
    ossim_valid($department, OSS_ALPHA, OSS_PUNC, OSS_AT, OSS_NULLABLE,  'illegal:'._("Department"));

    if (ossim_error()) {
            die(ossim_error());
    }

    if( !Session::am_i_admin() )
    {
        require_once("ossim_error.inc");
        $error = new OssimError;
        $error->display("ONLY_ADMIN");
    }

    /* check OK, insert into DB */
    elseif (POST("insert")) {

        require_once ('ossim_db.inc');
        require_once ('ossim_acl.inc');
        require_once ('classes/Session.inc');
        require_once ('classes/Net.inc');
        require_once ('classes/Sensor.inc');

        $perms = array();
        foreach ($ACL_MAIN_MENU as $menus) {
            foreach ($menus as $key => $menu) {
                if (POST($key) == "on")
                    $perms[$key] = true;
                else
                    $perms[$key] = false;
            }
        }
        $db = new ossim_db();
        $conn = $db->connect();

        $nets = "";
        for ($i = 0; $i < $nnets; $i++)
        {
            $net_name = POST("net$i");
            ossim_valid($net_name,  OSS_ALPHA, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("net$i"));
            if (ossim_error()) { die(ossim_error()); }

            if ($net_list = Net::get_list($conn, "WHERE name = '$net_name'"))
            {
                foreach ($net_list as $net)
                {
                    if ($nets == "")
                        $nets = $net->get_ips();
                    else
                        $nets .= "," . $net->get_ips();
                }
            }
        }
        $sensors = "";
        for ($i = 0; $i < $nsensors; $i++)
        {
            ossim_valid(POST("sensor$i"), OSS_LETTER, OSS_DIGIT, OSS_DOT, OSS_NULLABLE, 'illegal:'._("sensor$i"));
            if (ossim_error()) { die(ossim_error()); }
            if ($sensors == "")
                $sensors = POST("sensor$i");
            else
                $sensors .= "," . POST("sensor$i");
        }

        Session::update ($conn, $user, $name, $email, $perms, $nets, $sensors,
        $company, $department);

        $db->close($conn);
?>
    <p> <?php echo gettext("User succesfully updated"); ?> </p>
    <p><a href="users.php"> <?php echo gettext("Back"); ?> </a></p>
<?php
    }
?>


</body>
</html>

