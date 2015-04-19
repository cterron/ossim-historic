<?php
require_once 'classes/Upgrade_base.inc';
/*
 */

class upgrade_099rc4 extends upgrade_base
{

    // Normalize MAC (bad entry make problems in MAC anomalies)
    function start_upgrade()
    {
        $conn = &$this->conn;
        $snort = &$this->snort;
                
        $conn->StartTrans();
        $sql = "SELECT * FROM host_mac";
        if (!$rs = $conn->Execute($sql)) {
            die("Error was:<br>\n<b>".$conn->ErrorMsg()."</b>");
        }

        while (!$rs->EOF) {
            $mac = $rs->fields['mac'];
            if ($mac != "") {
                $ip = $rs->fields['ip'];
                $date = $rs->fields['date'];
                $sensor = $rs->fields['sensor'];

                $new_mac = strtoupper(vsprintf("%02s:%02s:%02s:%02s:%02s:%02s", split(":", $mac))); 

                $sql = 'UPDATE host_mac SET mac=? WHERE ip=? AND date=? AND sensor=?';
                $params = array($new_mac, $ip, $date, $sensor);
                $conn->Execute($sql, $params);
            }
            $rs->MoveNext();
        }
        $res = $conn->CompleteTrans();
        if (!$res) {
            die("Transacion failed: ". $conn->ErrorMsg());
        }

        /* Snort table changes */
        $sql = "ALTER TABLE ossim_event ADD COLUMN plugin_id INTEGER NOT NULL";
        if (!$snort->Execute($sql)) {
            print("Error was:<b>".$snort->ErrorMsg()."</b><br>");
        }

        $sql = "ALTER TABLE ossim_event ADD COLUMN plugin_sid INTEGER NOT NULL";
        if (!$snort->Execute($sql)) {
            print("Error was:<b>".$snort->ErrorMsg()."</b><br>");
        }

        $sql = "CREATE TABLE extra_data (
        sid             INT8 NOT NULL,
        cid             INT8 NOT NULL,
        filename        varchar(255),
        username        varchar(255),
        password        varchar(255),
        userdata1       varchar(255),
        userdata2       varchar(255),
        userdata3       varchar(255),
        userdata4       varchar(255),
        userdata5       varchar(255),
        userdata6       varchar(255),
        userdata7       varchar(255),
        userdata8       varchar(255),
        userdata9       varchar(255), 
        PRIMARY KEY (sid, cid)
);";
        if (!$snort->Execute($sql)) {
            print("Error was:<b>".$snort->ErrorMsg()."</b><br>");
        }

        return true;
    }
    
}
?>
