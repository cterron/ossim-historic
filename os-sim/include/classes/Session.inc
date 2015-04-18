<?php

require_once ('ossim_db.inc');
require_once ('ossim_conf.inc');
require_once ('ossim_acl.inc');

class Session {

    var $login;
    var $name;
    var $pass;
    var $nets;

    function Session ($login, $pass, $allowed_nets, $name = "")
    {
        $this->login = $login;
        $this->name = $name;
        $this->allowed_nets = $allowed_nets;
        $this->pass = $pass;
    }

    function get_login() { return $this->login; }
    function get_name() { return $this->name; }
    function get_allowed_nets() { return $this->allowed_nets; }

    function get_list($conn, $args = "")
    {
        $query = "SELECT * FROM users $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $list[] = new Session ($rs->fields["login"],
                                       $rs->fields["pass"],
                                       $rs->fields["allowed_nets"],
                                       $rs->fields["name"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }


    function login()
    {
        $db = new ossim_db();
        $conn = $db->connect();

        $pass = md5($this->pass);

        $query = "SELECT * FROM users WHERE login = '" .
            $this->login . "' AND pass = '" . $pass . "'";

        if ($rs = &$conn->Execute($query) && (!$rs->EOF)) 
        {
            session_start();
            $_SESSION["_user"] = $this->login;
            $db->close($conn);
            return True;
        }

        $db->close($conn);
        return False;
    }

    function logout()
    {
        session_start();
        unset($_SESSION["_user"]);
    }

    function logcheck($menu, $submenu, $login_location = "../session/login.php")
    {
    	  $user = $_POST["user"];
    	  $pass = $_POST["pass"];

		  if ($user && $pass) {
		  		$sess = new Session($user, $pass, "");
		  		$sess->login();
		  }
    
        session_start();

        $dest = $_SERVER["PHP_SELF"];
        if ( ! isset($_SESSION["_user"]) ) {
            header("Location: $login_location?dest=$dest");
            exit;
        }

        $conf = new ossim_conf();
        $phpgacl = $conf->get_conf("phpgacl_path");
        require_once ("$phpgacl" . "gacl.class.php");
        require_once ("$phpgacl" . "gacl_api.class.php");

        $gacl = new gacl();

        if (! $gacl->acl_check($menu,
                               $submenu,
                               ACL_DEFAULT_USER_SECTION,
                               $_SESSION["_user"]))
        {

            if (! $gacl->acl_check(ACL_DEFAULT_DOMAIN_SECTION,
                                   ACL_DEFAULT_DOMAIN_ALL,
                                   ACL_DEFAULT_USER_SECTION,
                                   $_SESSION["_user"]))
            {
                echo "<p align=\"center\">
                    <b>You don't have permission to see this page</b></p>";
                echo "<p align=\"center\">[ User: " . $_SESSION["_user"] . "]";
                exit;
            }
        }
    }

    function logcheck_ext($menu, $submenu, $axo_sec, $axo_val, $login_location = "../session/login.php")
    {
    	  $user = $_POST["user"];
    	  $pass = $_POST["pass"];

		  if ($user && $pass) {
		  		$sess = new Session($user, $pass, "");
		  		$sess->login();
		  }
    
        session_start();

        $dest = $_SERVER["PHP_SELF"];
        if ( ! isset($_SESSION["_user"]) ) {
            header("Location: $login_location?dest=$dest");
            exit;
        }

        $conf = new ossim_conf();
        $phpgacl = $conf->get_conf("phpgacl_path");
        require_once ("$phpgacl" . "gacl.class.php");
        require_once ("$phpgacl" . "gacl_api.class.php");

        $gacl = new gacl();

        if (! $gacl->acl_check($menu,
                               $submenu,
                               ACL_DEFAULT_USER_SECTION,
                               $_SESSION["_user"],
                               $axo_sec,
                               $axo_val))
        {

            if (! $gacl->acl_check(ACL_DEFAULT_DOMAIN_SECTION,
                                   ACL_DEFAULT_DOMAIN_ALL,
                                   ACL_DEFAULT_USER_SECTION,
                                   $_SESSION["_user"]))
            {
                echo "<p align=\"center\">
                    <b>You don't have permission to do this action</b></p>";
                echo "<p align=\"center\">[ User: " . $_SESSION["_user"] . "]";
                exit;
            }
        }
    }



    /* Wraps the acl_return_value() function */
    function getValue($user = "")
    {
        $conf = new ossim_conf();
        $phpgacl = $conf->get_conf("phpgacl_path");

        require_once ("$phpgacl" . "gacl.class.php");
        require_once ("$phpgacl" . "gacl_api.class.php");

        $gacl = new gacl();

        if (!$user) {
            session_start();
            $user = $_SESSION["_user"];
        }

        return $gacl->acl_return_value (ACL_DEFAULT_DOMAIN_SECTION,
                                        ACL_DEFAULT_DOMAIN_NETS,
                                        ACL_DEFAULT_USER_SECTION,
                                        $user);
    }

    function get_session_user()
    {
        session_start();
        return $_SESSION["_user"];
    }

    /* return true if we are admin user */
    function am_i_admin()
    {
        session_start();
        return ($_SESSION["_user"] == ACL_DEFAULT_OSSIM_ADMIN);
    }

    function is_expert()
    {
        $db = new ossim_db();
        $conn = $db->connect();

        $query = "SELECT value FROM phpgacl WHERE name = 'expert_mode'";
        if (!$rs = &$conn->Execute($query)){
            print $conn->ErrorMsg();
        } else {
            return $rs->fields["value"];
        }
    }

    function get_session_info()
    {
        session_start();
        if (isset($_SESSION["_user"]) )
        {
            $db = new ossim_db();
            $conn = $db->connect();

            $query = "SELECT * FROM users WHERE login = '" .
                $_SESSION["_user"] . "'";
            if (!$rs = &$conn->Execute($query)) {
                print $conn->ErrorMsg();
            } else {
                return $rs->fields;
            }
            $db->close($conn);
        }
        return NULL;
    }

    function insert ($conn, $login, $pass, $name, $perms, $nets)
    {

        global $ACL_MAIN_MENU;
        $pass = md5($pass);

        /* insert into OSSIM database */
        $query = "INSERT INTO users (login, name, pass, allowed_nets)
            VALUES ('$login', '$name', '$pass', '$nets')";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }

        /* insert into GACL database */
        $conf = new ossim_conf();
        $phpgacl = $conf->get_conf("phpgacl_path");
        require_once ("$phpgacl" . "gacl.class.php");
        require_once ("$phpgacl" . "gacl_api.class.php");

        $gacl_api = new gacl_api();

        $gacl_api->add_object (ACL_DEFAULT_USER_SECTION, /* section */
                               $login,       /* name  */
                               $login,       /* login */
                                0, 0,
                               'ARO');

        $gacl_api->add_group_object(
            $gacl_api->get_group_id(ACL_DEFAULT_USER_GROUP),
            ACL_DEFAULT_USER_SECTION,
            $login,
            'ARO');


        /* set perms */
        $aro_ids = array (ACL_DEFAULT_USER_SECTION => array($login));

        foreach ($ACL_MAIN_MENU as $aco_section => $menus)
        {
            foreach ($menus as $key => $menu)
            {
                $gacl_api->add_acl(
                    array($aco_section => array ($key)),
                    $aro_ids,
                    NULL,
                    NULL,
                    NULL,
                    $perms[$key],
                    True);
            }
        }

        /* set allowed nets */
        $gacl_api->add_acl(
                array(ACL_DEFAULT_DOMAIN_SECTION => array (
                    ACL_DEFAULT_DOMAIN_NETS)),
                $aro_ids,
                NULL,
                NULL,
                NULL,
                True,
                True,
                $nets);
    }

    function update ($conn, $login, $name, $perms, $nets)
    {

        global $ACL_MAIN_MENU;

        /* modify OSSIM database */
        $query = "UPDATE users SET name = '$name', allowed_nets = '$nets' 
            WHERE login = '$login';";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }

        /* update GACL database */
        $conf = new ossim_conf();
        $phpgacl = $conf->get_conf("phpgacl_path");
        require_once ("$phpgacl" . "gacl.class.php");
        require_once ("$phpgacl" . "gacl_api.class.php");

        $gacl_api = new gacl_api();

        $aro_ids = array (ACL_DEFAULT_USER_SECTION => array($login));

        /* update perms */
        foreach ($ACL_MAIN_MENU as $aco_section => $menus)
        {
            foreach ($menus as $key => $menu)
            {
                /* get ACL id */
                if ($acl_ids = $gacl_api->search_acl (
                                    $aco_section,
                                    $key,
                                    ACL_DEFAULT_USER_SECTION,
                                    $login,
                                    False, False, False, False, False))
                {
                    foreach ($acl_ids as $acl_id) /* must be only one id */
                    {
                        $gacl_api->edit_acl(
                            $acl_id,
                            array($aco_section => array ($key)),
                            $aro_ids,
                            NULL,
                            NULL,
                            NULL,
                            $perms[$key],
                            True);
                    }
                } 
                
                /* there is no menu entry, insert it */
                else 
                {
                    $gacl_api->add_acl(
                        array($aco_section => array ($key)),
                        $aro_ids,
                        NULL,
                        NULL,
                        NULL,
                        $perms[$key],
                        True);
                }
            }
        }
       
        /* update allowed nets */
        if ($acl_ids = $gacl_api->search_acl (
                            ACL_DEFAULT_DOMAIN_SECTION,
                            ACL_DEFAULT_DOMAIN_NETS,
                            ACL_DEFAULT_USER_SECTION,
                            $login,
                            False, False, False, False, False))
        {
            foreach ($acl_ids as $acl_id) /* must be only one id */
            {
                $gacl_api->edit_acl(
                    $acl_id,
                    array(ACL_DEFAULT_DOMAIN_SECTION => array (
                        ACL_DEFAULT_DOMAIN_NETS)),
                    $aro_ids,
                    NULL,
                    NULL,
                    NULL,
                    True,
                    True,
                    $nets);
            }
        }
        
    }

    function changepass ($conn, $login, $pass)
    {
        $pass = md5($pass);
        $query = "UPDATE users SET
            pass = '$pass' WHERE login = '$login'";
        if (
        $conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete ($conn, $login)
    {
        global $ACL_MAIN_MENU;

        if ($login == ACL_DEFAULT_OSSIM_ADMIN) {
            print "<p>Can't remove default user</p>";
            exit;
        }

        $query = "DELETE FROM users WHERE login = '$login'";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }

        $conf = new ossim_conf();
        $phpgacl = $conf->get_conf("phpgacl_path");

        require_once ("$phpgacl" . "gacl.class.php");
        require_once ("$phpgacl" . "gacl_api.class.php");

        $gacl_api = new gacl_api();


        /* delete ACLs */
        if ($acl_ids = $gacl_api->search_acl(FALSE,
                                         FALSE,
                                         ACL_DEFAULT_USER_SECTION,
                                         $login,
                                         False, False, False, False, False))
        {
            foreach ($acl_ids as $id)
            {
                $gacl_api->del_acl($id);
            }
        }

        /* delete user object */
        $gacl_api->del_object(
                $gacl_api->get_object_id(ACL_DEFAULT_USER_SECTION,
                                         $login,
                                         "ARO"),
                "ARO",
                True);
    }
}

?>
