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

  <h1> <?php echo gettext("New User"); ?> </h1>

<?php

    /* check params */
    if (($_POST["insert"]) &&
        (!$_POST["user"]  || !$_POST["name"]))
    {
        echo "<p align=\"center\">Please, complete all the fields</p>";
        exit();
    }

    /* check OK, insert into DB */
    elseif ($_POST["insert"]) {

        require_once ('ossim_db.inc');
        require_once ('ossim_acl.inc');
        require_once ('classes/Session.inc');
        require_once ('classes/Net.inc');

        $user  = $_POST["user"];
        $name  = $_POST["name"];
        $nnets = $_POST["nnets"];

        $perms = Array();
        foreach ($ACL_MAIN_MENU as $menus) {
            foreach ($menus as $key => $menu) {
                if ($_POST[$key] == "on")
                    $perms[$key] = True;
                else
                    $perms[$key] = False;
            }
        }

        $db = new ossim_db();
        $conn = $db->connect();

        $nets = "";
        for ($i = 0; $i < $nnets; $i++)
        {
            $net_name = $_POST["net$i"];
            if ($net_list = Net::get_list($conn, "WHERE name = '$net_name'"))
            {
                foreach ($net_list as $net)
                {
                    if (!$nets)
                        $nets = $net->get_ips();
                    else
                        $nets .= "," . $net->get_ips();
                }
            }
        }

        Session::update ($conn, $user, $name, $perms, $nets);

        $db->close($conn);
?>
    <p> <?php echo gettext("User succesfully updated"); ?> </p>
    <p><a href="users.php"> <?php echo gettext("Back"); ?> </a></p>
<?php
    }
?>


</body>
</html>

