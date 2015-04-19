<?php
require_once 'classes/Upgrade_base.inc';
/*
 * 0.9.8 to 0.9.9 upgrade script
 */

class upgrade_099rc1 extends upgrade_base
{

    // new fields added to table Incident. Before that, the values were
    // extracted from the latest inserted incident_ticket
    // These are: in_charge, last_update, status and priority
    function end_upgrade()
    {
        $conn = &$this->conn;
        
        $conn->StartTrans();
        $sql = "SELECT id, in_charge, last_update, status, priority, date " .
               "FROM incident";
        if (!$rs = $conn->Execute($sql)) {
            die("Error was:<br>\n<b>".$conn->ErrorMsg()."</b>");
        }
        
        while (!$rs->EOF) {
            $id = $rs->fields['id'];
            $date = $last_update = $rs->fields['date']; // incident creation time
            
            $in_charge = $rs->fields['in_charge'];
            $last_update = $rs->fields['last_update'];
            $status = $rs->fields['status'];
            $priority = $rs->fields['priority'];
            
            //
            // In charge
            //
            if (empty($in_charge)) {
                $sql = "SELECT in_charge, transferred FROM incident_ticket
                        WHERE incident_id=$id ORDER BY id DESC LIMIT 1";
                if (!$rs2 = $conn->Execute($sql)) die($conn->ErrorMsg());
                if ($rs2->EOF) {
                    $in_charge = ACL_DEFAULT_OSSIM_ADMIN;
                } else {
                    $in_charge   = $rs2->fields["in_charge"];
                    $transferred = $rs2->fields["transferred"];
                    if ($transferred) $in_charge = $transferred;
                }
                $rs2->close();
            }
            
            //
            // Creation date
            //
            $sql = "SELECT date FROM incident_ticket
                    WHERE incident_id=$id ORDER BY id ASC LIMIT 1";
            if (!$rs2 = $conn->Execute($sql)) die($conn->ErrorMsg());
            if (!$rs2->EOF) {
                $first_ticket = $rs2->fields['date'];
                // workarround old bug (autoupdate TIMESTAMP fields)
                if (strtotime($first_ticket) < strtotime($date)) {
                    $date = $first_ticket;
                }
                $rs2->close();
            }
            
            //
            // Last update
            //
            if ($last_update == '0000-00-00 00:00:00') {
                $sql = "SELECT date FROM incident_ticket " .
                       "WHERE incident_id = $id ORDER BY id DESC";
                if (!$rs2 = $conn->Execute($sql)) die($conn->ErrorMsg());
                // use incident creation date (computed before) when no ticket
                if (!empty($rs2->fields['date'])) {
                    $last_update = $rs2->fields['date'];
                }
                $rs2->close();
            }
            
            //
            // Status
            //
            if ($status == 'Open') {
                $sql = "SELECT status FROM incident_ticket
                       WHERE incident_id = $id ORDER BY id DESC";
                if (!$rs2 = $conn->Execute($sql)) die($conn->ErrorMsg());
                if (!empty($rs2->fields['status'])) {
                    $status = $rs2->fields['status'];
                }
                $rs2->close();
            }
            
            //
            // Priority
            //
            $sql = "SELECT priority FROM incident_ticket
                    WHERE incident_id = $id ORDER BY id DESC";
            if (!$rs2 = $conn->Execute($sql)) die($conn->ErrorMsg());
            if (!empty($rs2->fields['priority'])) {
                $priority = $rs2->fields['priority'];
            }
            if ($priority > 10) $priority = 10;
            if (empty($priority) || ($priority < 1)) $priority = 1;
            
            //
            // Upgrade fields
            //
            $sql = "UPDATE incident " .
                   "SET in_charge=?, date=?, last_update=?, status=?, priority=? ".
                   "WHERE id = $id";
            $parms = array($in_charge, $date, $last_update, $status, $priority);
            if (!$conn->Execute($sql, $parms)) die($conn->ErrorMsg());
            $rs->MoveNext();
        }
        $conn->CompleteTrans();
        if ($conn->HasFailedTrans()) {
            return ossim_set_error($conn->ErrorMsg());
        }
        //
        // Reload ACLS
        //
        $this->reload_acls();
        return true;
    }
    
}
?>
