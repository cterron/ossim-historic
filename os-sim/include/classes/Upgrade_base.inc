<?php

class upgrade_base
{
    /*
     * These properties are set by the Upgrade class,
     * which creates this object
     */
    var $upgrades_path;
    var $conn;
    var $snort;
    var $conf;
    var $about;
    var $ossim_dbtype;
    var $ossim_current_version;
    var $ossim_schema_version;
    
    function start_upgrade()
    {
        return true;
    }
    
    function end_upgrade()
    {
        return true;
    }

    /*
     * Helper method for reloading the users ACL database
     * (needed when new perms are added)
     */
    function reload_acls()
    {
        $proto = isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] == 'on' ? 'https' : 'http';
        $port  = $_SERVER['SERVER_PORT'];
        $host = $_SERVER['HTTP_HOST'];
        $path = $this->conf->get_conf('ossim_link');
        $ossim_acl = $proto.'://'.$host.':'.$port.'/'.$path.'/setup/ossim_acl.php';
        echo _("Executing").": $ossim_acl<br>";
        include $ossim_acl;
    }

}
?>
