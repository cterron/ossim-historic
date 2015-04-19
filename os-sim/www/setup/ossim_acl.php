<?php

include ('ossim_conf.inc');
include ('ossim_acl.inc');

$conf = $GLOBALS["CONF"];
$phpgacl = $conf->get_conf("phpgacl_path");

require_once ("$phpgacl/gacl.class.php");
require_once ("$phpgacl/gacl_api.class.php");

$gacl_api = new gacl_api($ACL_OPTIONS);

/* Domain access */
echo gettext("Setting up domain access")."...<br/>";
$gacl_api->add_object_section (ACL_DEFAULT_DOMAIN_SECTION,
                               ACL_DEFAULT_DOMAIN_SECTION,
                               1, 0, 'ACO');
echo "  * ".gettext("Users")."...<br/>";
$gacl_api->add_object (ACL_DEFAULT_DOMAIN_SECTION,
                       ACL_DEFAULT_DOMAIN_ALL,
                       ACL_DEFAULT_DOMAIN_ALL,
                       1, 0, 'ACO');
$gacl_api->add_object (ACL_DEFAULT_DOMAIN_SECTION,
                       ACL_DEFAULT_DOMAIN_LOGIN,
                       ACL_DEFAULT_DOMAIN_LOGIN,
                       2, 0, 'ACO');
echo "  * ".gettext("Networks")."...<br/>";
$gacl_api->add_object (ACL_DEFAULT_DOMAIN_SECTION,
                       ACL_DEFAULT_DOMAIN_NETS,
                       ACL_DEFAULT_DOMAIN_NETS,
                       3, 0, 'ACO');
echo "  * ".gettext("Sensors")."...<br/><br/>";
$gacl_api->add_object (ACL_DEFAULT_DOMAIN_SECTION,
                       ACL_DEFAULT_DOMAIN_SENSORS,
                       ACL_DEFAULT_DOMAIN_SENSORS,
                       4, 0, 'ACO');

/* Menu access */
$menu_count = 10;
$submenu_count = 1;


echo "Setting up Menu access...<br/>";
foreach ($ACL_MAIN_MENU as $menu_name => $menu)
{

    $gacl_api->add_object_section ($menu_name,
                                   $menu_name,
                                   $menu_count++,
                                   0,
                                   'ACO');

    foreach ($menu as $submenu_name => $submenu)
    {
        echo "  * " . $submenu["name"] . " ...<br/>";

        $gacl_api->add_object ($menu_name,
                               $submenu_name,
                               $submenu_name,
                               $submenu_count++,
                               0,
                               "ACO");
    }

    $submenu_count = 1;
}


/* Groups */
echo "<br/>Setting up default admin user...<br/><br/>";
$groups['ossim'] = $gacl_api->add_group('ossim',
                                        'OSSIM',
                                        0,
                                        'ARO');
$groups['users'] = $gacl_api->add_group(ACL_DEFAULT_USER_GROUP,
                                        'Users',
                                        $groups['ossim'],
                                        'ARO');
/* Default User */
$gacl_api->add_object_section ('Users',
                               ACL_DEFAULT_USER_SECTION,
                               1,
                               0,
                               'ARO');
$gacl_api->add_object (ACL_DEFAULT_USER_SECTION,
                       'Admin',
                       ACL_DEFAULT_OSSIM_ADMIN,
                       1,
                       0,
                       'ARO');

$gacl_api->add_acl (
    array (ACL_DEFAULT_DOMAIN_SECTION => array (ACL_DEFAULT_DOMAIN_ALL)),
    array (ACL_DEFAULT_USER_SECTION => array (ACL_DEFAULT_OSSIM_ADMIN))
);

?>
<?
// The upgrade system at include/classes/Upgrade_base.inc includes
// that file like: include 'http://foo/setup/ossim_acl.php'
// In this case, there is not HTTP_REFERER and btw we don't want to show
// this "go back" link.
if (isset($_SERVER['HTTP_REFERER'])) { ?>
<a href="<?php echo $_SERVER['HTTP_REFERER'];?>"> <?php echo gettext("Back"); ?> </a>
<? } ?>
