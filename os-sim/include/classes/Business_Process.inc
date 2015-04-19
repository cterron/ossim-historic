<?php

class BP_Asset {
    
    var $id;
    var $name;
    var $description;
    var $members;
    var $responsibles;
    
    function BP_Asset($id, $name, $description, $members, $responsibles)
    {
        $this->id = $id;
        $this->name = $name;
        $this->description = $description;
        $this->members = $members;
        $this->responsibles = $responsibles;
    }
    
    /*
     * static
     */
    function get($conn, $id)
    {
        // Name and Description
        $sql = "SELECT name, description FROM bp_asset WHERE id=?";
        $row = $conn->GetRow($sql, array($id));
        if ($row === false) {
            die($conn->ErrorMsg());
        }
        $name = $row['name'];
        $description = $row['description'];

        // Responsibles
        $sql = "SELECT u.login, u.name, u.email
                FROM users u, bp_asset_responsible resp
                WHERE u.login = resp.login AND resp.asset_id = ?
                ORDER BY u.name";
        $data = $conn->GetAll($sql, array($id));
        if ($data === false) {
            die($conn->ErrorMsg());
        }
        $responsibles = $data;

        // Members
        $sql = "SELECT
                    mem.member as name,
                    mem.member_type as type,
                    status.status_date as date,
                    status.measure_type as measure_type,
                    status.severity as severity
                FROM
                    bp_asset_member mem
                LEFT JOIN bp_member_status AS status ON mem.member = status.member
                WHERE
                    mem.asset_id=?
                ORDER BY mem.member_type, mem.member";
        if (!$rs = $conn->Execute($sql, array($id))) {
            die($conn->ErrorMsg());
        }
        $members = array();
        while (!$rs->EOF) {
            $members[] = $rs->fields;
            $rs->MoveNext();
        }
        return new BP_Asset($id, $name, $description, $members, $responsibles);
    }
    
    function get_name() { return $this->name; }
    function get_description() { return $this->description; }
    function get_members() { return $this->members; }
    function get_responsibles() { return $this->responsibles; }
    
    function get_measure_type_str($measure)
    {
        switch ($measure) {
            case 'incident':      return _("Incidents");
            case 'host_alarm':     return _("Alarms");
            case 'host_metric':   return _("Metrics");
            case 'host_vulnerability': return _("Vulnerabilities");
            case 'host_incident':   return _("Incidents");
            case 'host_incident_alarm': return _("Incident alarm");
            case 'host_incident_event': return _("Incident event");
            case 'host_incident_metric': return _("Incident metric");
            case 'host_incident_anomaly': return _("Incident anomaly");
            case 'host_incident_vulns': return _("Incident vuln.");
            case 'net_metric': return _("Metrics");
            case 'net_vulnerability': return _("Vulnerabilties");
            default: return _("Unknown measure type");
        }
    }
    
}

?>
