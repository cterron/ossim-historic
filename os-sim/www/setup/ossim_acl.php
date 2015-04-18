<?php

include ('ossim_conf.inc');
include ('ossim_acl.inc');

$conf = new ossim_conf();
$phpgacl = $conf->get_conf("phpgacl_path");

include ("$phpgacl/gacl.class.php");
include ("$phpgacl/gacl_api.class.php");

$gacl_api = new gacl_api();

/* Domain access */
echo "Setting up domain access...<br/><br/>";
$gacl_api->add_object_section (ACL_DEFAULT_DOMAIN_SECTION,
                               ACL_DEFAULT_DOMAIN_SECTION,
                               1, 0, 'ACO');
$gacl_api->add_object (ACL_DEFAULT_DOMAIN_SECTION,
                       ACL_DEFAULT_DOMAIN_ALL,
                       ACL_DEFAULT_DOMAIN_ALL,
                       1, 0, 'ACO');
$gacl_api->add_object (ACL_DEFAULT_DOMAIN_SECTION,
                       ACL_DEFAULT_DOMAIN_LOGIN,
                       ACL_DEFAULT_DOMAIN_LOGIN,
                       2, 0, 'ACO');
$gacl_api->add_object (ACL_DEFAULT_DOMAIN_SECTION,
                       ACL_DEFAULT_DOMAIN_NETS,
                       ACL_DEFAULT_DOMAIN_NETS,
                       3, 0, 'ACO');

/* Menu access */
$menu_count = 10;
$submenu_count = 1;

foreach ($ACL_MAIN_MENU as $menu_name => $menu)
{

    $gacl_api->add_object_section ($menu_name,
                                   $menu_name,
                                   $menu_count++,
                                   0,
                                   'ACO');

    foreach ($menu as $submenu_name => $submenu)
    {
        print "Setting up " . $submenu["name"] . " ...<br/>";

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

?><a href="<?php echo $_SERVER['HTTP_REFERER'];?>"> Back </a>
