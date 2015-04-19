<?php
require_once 'classes/Locale.inc';
require_once 'classes/Host.inc';
require_once 'classes/Net.inc';
require_once 'classes/Log_action.inc';

require_once 'ossim_db.inc';
require_once 'ossim_acl.inc';

/* global configuration */
require_once 'ossim_conf.inc';

session_start();


class Session {

    var $login;
    var $name;
    var $pass;
    var $nets;
    var $company;
    var $department;
    
    function Session ($login, $pass, $allowed_nets, $name = "", $email = "",
    $company = "", $department = "")
    {
        $this->login = strtolower($login);
        $this->name  = $name;
        $this->email = $email;
        $this->allowed_nets = $allowed_nets;
        $this->pass  = $pass;
        $this->company = $company;
        $this->department = $department;
    }

    function get_login() { return $this->login; }
    function get_name() { return $this->name; }
    function get_email() { return $this->email; }
    function get_allowed_nets() { return $this->allowed_nets; }
    function get_company() { return $this->company; }
    function get_department() { return $this->department; }

    function get_list($conn, $args = "")
    {
        $list = array();
        $query = OssimQuery("SELECT * FROM users $args");
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Session ($rs->fields["login"],
                                       $rs->fields["pass"],
                                       $rs->fields["allowed_nets"],
                                       $rs->fields["name"],
                                       $rs->fields["email"],
                                       $rs->fields["company"],
                                       $rs->fields["department"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    /*
     * Gets a Session object from the user currently logged
     */
    function get_me($conn)
    {
        $user = Session::get_session_user();
        list($me) = Session::get_list($conn, "WHERE login='$user'");
        return $me;
    }

    function login()
    {
        $db = new ossim_db();
        $conn = $db->connect();

        $pass = md5($this->pass);
        $login = $this->login;

        $sql = "SELECT * FROM users WHERE login = ? AND pass = ?";
        $params = array($login, $pass);

        if ($rs = &$conn->Execute($sql, $params) && (!$rs->EOF)) 
        {
            
            $_SESSION["_user"] = $this->login;
            $db->close($conn);
            return true;
        }

        $db->close($conn);
        return false;
    }

    function logout()
    {
        session_destroy();
    }

    function menu_perms($menu, $submenu)
    {
        $gacl = $GLOBALS['ACL'];

        if (!isset($_SESSION["_user"]) )
            return true;
     
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
                return false;
            }
        }
        
        return true;
    }

    function logcheck($menu, $submenu, $login_location = false)
    {
        $gacl = $GLOBALS['ACL'];
        if (array_key_exists('user', $_POST))
            $user = $_POST["user"];
        
        if (array_key_exists('pass', $_POST))
    	    $pass = $_POST["pass"];

		if (isset($user) && isset($pass)) {
		    $sess = new Session($user, $pass, "");
		  	$sess->login();
		}
        
        if (!$login_location) {
            $conf = $GLOBALS['CONF'];
            $ossim_link = $conf->get_conf("ossim_link");
            $login_location = $ossim_link . '/session/login.php'; 
        }
        
        $dest = $_SERVER["PHP_SELF"];
        if ( ! isset($_SESSION["_user"]) ) {
            header("Location: $login_location?dest=$dest");
            exit;
        }
        
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
                echo "<p align=\"center\">[ User: " . $_SESSION["_user"] . "]</p>";
                echo "<p align=\"center\">[ <a href=\"session/login.php?action=logout\"
                      title=\"Logout\"><font color=\"black\">Logout</font></a> ]</p>";
                exit;
            }
        }
    }

    function logcheck_ext($menu, $submenu, $axo_sec, $axo_val, $login_location = "../session/login.php")
    {
        $gacl = $GLOBALS['ACL'];
        $user = $_POST["user"];
        $pass = $_POST["pass"];
        
        if ($user && $pass) {
        	$sess = new Session($user, $pass, "");
        	$sess->login();
        }
    
        $dest = $_SERVER["PHP_SELF"];
        if ( ! isset($_SESSION["_user"]) ) {
            header("Location: $login_location?dest=$dest");
            exit;
        }

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


    /* 
     * Backwards compatibility with allowedNets function 
     * (acid patches, old ossim-framework code, etc)
     */
    function getValue($user = "")
    {
        return Session::allowedNets($user);
    }


    /* Wraps the acl_return_value() function */
    function allowedNets($user = "")
    {
        $gacl = $GLOBALS['ACL'];
        static $allowedNetsCache;
    
        if (!$user)
            $user = Session::get_session_user();

        if (isset($allowedNetsCache["$user"]))
            return $allowedNetsCache["$user"];

        $allowedNetsCache["$user"] =
            $gacl->acl_return_value (ACL_DEFAULT_DOMAIN_SECTION,
                                     ACL_DEFAULT_DOMAIN_NETS,
                                     ACL_DEFAULT_USER_SECTION,
                                     $user);
        return $allowedNetsCache["$user"];
    }

    function allowedSensors($user = "")
    {
        $gacl = $GLOBALS['ACL'];
        static $allowedSensorsCache;
    
        if (!$user)
            $user = Session::get_session_user();

        if (isset($allowedSensorsCache["$user"]))
            return $allowedSensorsCache["$user"];

        $allowedSensorsCache["$user"] =
            $gacl->acl_return_value (ACL_DEFAULT_DOMAIN_SECTION,
                                     ACL_DEFAULT_DOMAIN_SENSORS,
                                     ACL_DEFAULT_USER_SECTION,
                                     $user);

        return $allowedSensorsCache["$user"];
    }

    function hostAllowed($conn, $host = "ANY", $user = "")
    {
        static $hostAllowedCache;
    
        if (!is_object($conn)) {
            echo "Missing database connection object.";
            exit();
        }
        
        if ($host == "ANY")
            return true;
   
        if (!$user)
            $user = Session::get_session_user();

        if (isset($hostAllowedCache["$user"]["$host"])) {
            return $hostAllowedCache["$user"]["$host"];
        }
        
        $allowed_nets = Session::allowedNets($user);
        $allowed_sensors = Session::allowedSensors($user);

        if ($allowed_nets) {
            if ( !(Net::isIpInNet($host, $allowed_nets)) ) {
                $hostAllowedCache["$user"]["$host"] = false;
                return false;
            }
        }

        if ($allowed_sensors) {
            $sensor_list = split(",", $allowed_sensors);
            if ( !(array_intersect(Host::get_related_sensors($conn, $host),
                                   $sensor_list)) )
            {
                $hostAllowedCache["$user"]["$host"] = false;
                return false;
            }
        }

        $hostAllowedCache["$user"]["$host"] = true;
        return true;
    }

    function groupAllowed($conn, $group = "ANY", $user = "")
    {
        static $groupAllowedCache;

        if (!is_object($conn)) {
            echo "Missing database connection object.";
            exit();
        }

        if ($group == "ANY")
            return true;

        if (!$user)
            $user = Session::get_session_user();

        if (isset($groupAllowedCache["$user"]["$group"])) {
            return $groupAllowedCache["$user"]["$group"];
        }

        $networks = Net_group::get_networks($conn, $group);

        foreach ($networks as $network){
        	if(Session::netAllowed($conn, $network->net_name, $user)){
                		$groupAllowedCache["$user"]["$group"] = true;
        		return true;
        	}
        }

        $groupAllowedCache["$user"]["$group"] = false;
        return false;
    }

    function netAllowed($conn, $net = "ANY", $user = "")
    {
        static $netAllowedCache;

        if (!is_object($conn)) {
            echo "Missing database connection object.";
            exit();
        }

        if ($net == "ANY")
            return true;
 
        if (!$user)
            $user = Session::get_session_user();

        if (isset($netAllowedCache["$user"]["$net"])) {
            return $netAllowedCache["$user"]["$net"];
        }
       
        $allowed_nets = Session::allowedNets($user);
        $allowed_sensors = Session::allowedSensors($user);

        if ($allowed_nets)
        {
            /* 
             * get ips from network $net 
             * don't use Net::get_list(), it's a recursive call:
             *  Net::get_list() -> Session::netAllowed() -> Net::get_list()
             */
            $sql = "SELECT ips FROM net WHERE name = ?";
            $params = array($net);
            if (!$rs = $conn->Execute($sql, $params)) {
                die($conn->ErrorMsg());
            }
            if (!$rs->EOF) {
                $ips = $rs->fields["ips"];
                $allowed_nets_array = split(',', $allowed_nets);
                if (!in_array($ips, $allowed_nets_array)) {
                    $netAllowedCache["$user"]["$net"] = false;
                    return false;
                }
            }
        }

        if ($allowed_sensors) 
        {
            $sensor_list = split(",", $allowed_sensors);
            if (!array_intersect(Net::get_related_sensors($conn, $net),
                                   $sensor_list))
            {
                $netAllowedCache["$user"]["$net"] = false;
                return false;
            }
        }

        $netAllowedCache["$user"]["$net"] = true;
        return true;
    }

    function get_session_user()
    {
        return @$_SESSION["_user"];
    }

    /* return true if we are admin user */
    function am_i_admin()
    {
        return ($_SESSION["_user"] == ACL_DEFAULT_OSSIM_ADMIN);
    }

    function is_expert()
    {
        $db = new ossim_db();
        $conn = $db->connect();

        $sql = "SELECT value FROM phpgacl WHERE name = 'expert_mode'";
        if (!$rs = &$conn->Execute($sql)){
            print $conn->ErrorMsg();
        } else {
            return $rs->fields["value"];
        }
    }

    function get_session_info()
    {
        if (isset($_SESSION["_user"]) )
        {
            $db = new ossim_db();
            $conn = $db->connect();

            $sql = "SELECT * FROM users WHERE login = '" . $_SESSION["_user"] . "'";
            if (!$rs = &$conn->Execute($sql)) {
                print $conn->ErrorMsg();
            } else {
                return $rs->fields;
            }
            $db->close($conn);
        }
        return NULL;
    }

    function insert ($conn, $login, $pass, $name, $email, 
                     $perms, $nets, $sensors, $company, $department)
    {
        global $ACL_MAIN_MENU;
        global $ACL_OPTIONS;
        $pass = md5($pass);

        /* bug with mrtg graphing? */
        $login = strtolower($login);

        /* insert into OSSIM database */
        $sql = "INSERT INTO users (login, name, pass, email, allowed_nets, company, department) VALUES (?, ?, ?, ?, ?, ?, ?)";
        $params = array($login, $name, $pass, $email, $nets, $company, $department);
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }

        /* insert into GACL database */
        
        $gacl_api = new gacl_api($ACL_OPTIONS);

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
                    true);
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
                true,
                true,
                $nets);

        /* set allowed sensors */
        $gacl_api->add_acl(
                array(ACL_DEFAULT_DOMAIN_SECTION => array (
                    ACL_DEFAULT_DOMAIN_SENSORS)),
                $aro_ids,
                NULL,
                NULL,
                NULL,
                true,
                true,
                $sensors);
                
        $infolog = array($login);
        Log_action::log(4,$infolog);
    }

    function update ($conn, $login, $name, $email, $perms, $nets, $sensors, $company, $department)
    {
        global $ACL_MAIN_MENU;
        global $ACL_OPTIONS;
        
        /* modify OSSIM database */
        $sql = "UPDATE users SET name = ?, allowed_nets = ?, email = ?, company = ?, department = ? WHERE login = ?";
        $params = array($name, $nets, $email, $company, $department, $login);
        
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }

        /* update GACL database */
        $gacl_api = new gacl_api($ACL_OPTIONS);

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
                                    false, false, false, false, false))
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
                            true);
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
                        true);
                }
            }
        }
       
        /* update allowed nets */
        if ($acl_ids = $gacl_api->search_acl (
                            ACL_DEFAULT_DOMAIN_SECTION,
                            ACL_DEFAULT_DOMAIN_NETS,
                            ACL_DEFAULT_USER_SECTION,
                            $login,
                            false, false, false, false, false))
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
                    true,
                    true,
                    $nets);
            }
        }

        /* update allowed sensors */
        if ($acl_ids = $gacl_api->search_acl (
                            ACL_DEFAULT_DOMAIN_SECTION,
                            ACL_DEFAULT_DOMAIN_SENSORS,
                            ACL_DEFAULT_USER_SECTION,
                            $login,
                            false, false, false, false, false))
        {
            foreach ($acl_ids as $acl_id) /* must be only one id */
            {
                $gacl_api->edit_acl(
                    $acl_id,
                    array(ACL_DEFAULT_DOMAIN_SECTION => array (
                        ACL_DEFAULT_DOMAIN_SENSORS)),
                    $aro_ids,
                    NULL,
                    NULL,
                    NULL,
                    true,
                    true,
                    $sensors);
            }
        }
        $infolog = array($login);
        Log_action::log(6,$infolog); 
        
    }

    function changepass ($conn, $login, $pass)
    {
        $pass = md5($pass);
        $sql = "UPDATE users SET pass = ? WHERE login = ?";
        $params = array($pass, $login);
        if ($conn->Execute($sql, $params) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        $infolog = array($login);
        Log_action::log(5,$infolog);  
    }

    function delete ($conn, $login)
    {
        global $ACL_MAIN_MENU;
        global $ACL_OPTIONS;
        
        if ($login == ACL_DEFAULT_OSSIM_ADMIN) {
            print "<p>Can't remove default user</p>";
            exit;
        }

        $sql = "DELETE FROM users WHERE login = ?";
        $params = array($login);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }

        $gacl_api = new gacl_api($ACL_OPTIONS);

        /* delete ACLs */
        if ($acl_ids = $gacl_api->search_acl(false,
                                         false,
                                         ACL_DEFAULT_USER_SECTION,
                                         $login,
                                         false, false, false, false, false))
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
                true);
        $infolog = array($login);        
        Log_action::log(3,$infolog);
    }
}

?>
