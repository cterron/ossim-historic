<?php

/**
 ** Common functions to DB management
 **/

require_once "ossim_conf.inc";
$conf = new ossim_conf();

$adodb_path = $conf->get_conf("adodb_path");
require_once ("$adodb_path/adodb.inc.php");

class ossim_db {

    /*
     *  Connect and close DB functions
     *  Read DB properties from /etc/ossim.conf
     */
    function connect () { 
        global $conf;

        $base = $conf->get_conf("ossim_base");
        $user = $conf->get_conf("ossim_user");
        $pass = $conf->get_conf("ossim_pass");
        $host = $conf->get_conf("ossim_host");

        $conn = &ADONewConnection('mysql');
        $conn->PConnect($host, $user, $pass, $base);
        return $conn;
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

