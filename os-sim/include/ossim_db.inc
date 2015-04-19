<?php

/**
 ** Common functions to DB management
 **/

require_once "ossim_conf.inc";
$conf = new ossim_conf();

$adodb_path = $conf->get_conf("adodb_path");
require_once ("$adodb_path/adodb.inc.php");

class ossim_db {

    var $conf;
    var $conn;

    /*
     *  Connect and close DB functions
     *  Read DB properties from /etc/ossim.conf
     */
    function ossim_db() 
    {
        require_once ('ossim_conf.inc');
        $this->conf = new ossim_conf();

        $adodb_path = $this->conf->get_conf("adodb_path");
        require_once ("$adodb_path/adodb.inc.php");

        $this->conn = null;
    }
     
    function connect () { 

        $base = $this->conf->get_conf("ossim_base");
        $user = $this->conf->get_conf("ossim_user");
        $pass = $this->conf->get_conf("ossim_pass");
        $host = $this->conf->get_conf("ossim_host");
        $type = $this->conf->get_conf("ossim_type");

        $this->conn = &ADONewConnection($type);
        $this->conn->NConnect($host, $user, $pass, $base);
        return $this->conn;
    }

    function snort_connect() {

        $base = $this->conf->get_conf("snort_base");
        $user = $this->conf->get_conf("snort_user");
        $pass = $this->conf->get_conf("snort_pass");
        $host = $this->conf->get_conf("snort_host");
        $type = $this->conf->get_conf("snort_type");

        $this->conn = &ADONewConnection($type);
        $this->conn->NConnect($host, $user, $pass, $base);
        return $this->conn;
    }

    function opennms_connect() {

        $base = $this->conf->get_conf("opennms_base");
        $user = $this->conf->get_conf("opennms_user");
        $pass = $this->conf->get_conf("opennms_pass");
        $host = $this->conf->get_conf("opennms_host");
        $type = $this->conf->get_conf("opennms_type");

        $this->conn = &ADONewConnection($type);
        $this->conn->NConnect($host, $user, $pass, $base);
        return $this->conn;
    }

    function phpgacl_connect() {

        $base = $this->conf->get_conf("phpgacl_base");
        $user = $this->conf->get_conf("phpgacl_user");
        $pass = $this->conf->get_conf("phpgacl_pass");
        $host = $this->conf->get_conf("phpgacl_host");
        $type = $this->conf->get_conf("phpgacl_type");

        $this->conn = &ADONewConnection($type);
        if ($this->conn->NConnect($host, $user, $pass, $base))
            return $this->conn;
        else
            return NULL;
    }

    function close($conn) {
        $conn->Close();
    }


    /*
     *  Common functions
     */
    function max_val($conn, $column, $table) {
    
        $query = "SELECT max($column) FROM $table;";
        
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } elseif (!$rs->EOF) {
            return $rs->fields["max($column)"];
        }
    }

    function get_order($column, $order) {
        if ($order == $column) {
            return "$order DESC";
        } else {
            return $column;
        }
    }

}

?>
