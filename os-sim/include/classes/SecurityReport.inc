<?php

require_once ('ossim_db.inc');

class SecurityReport
{
    var $ossim_conn;
    var $snort_conn;

    function SecurityReport()
    {
        $db = new ossim_db();
        $this->ossim_conn = $db->connect();
        $this->snort_conn = $db->snort_connect();
    }

    function AttackHost($ip_target, $limit = 10)
    {
        require_once ('classes/Host.inc');

        $list = array();

        $query = "SELECT count($ip_target) AS occurrences,
                  inet_ntoa($ip_target) as ip
                  FROM acid_event
                  GROUP BY $ip_target
                  ORDER BY occurrences DESC LIMIT $limit;";
        if (!$rs = &$this->snort_conn->Execute($query)) {
            print $this->snort_conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $tmp_array = array ($rs->fields["ip"], 
                                    $rs->fields["occurrences"]);
                array_push($list, $tmp_array);
                $rs->MoveNext();
            }
        }
        return $list;
    }
    
    function Alerts($limit = 10)
    {
        $list = array();

        $query = "SELECT count(sig_name) AS occurrences, sig_name
                  FROM acid_event GROUP BY sig_name
                  ORDER BY occurrences DESC LIMIT $limit;";
        if (!$rs = &$this->snort_conn->Execute($query)) {
            print $this->snort_conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $tmp_array = array ($rs->fields["sig_name"], 
                                    $rs->fields["occurrences"]);
                array_push($list, $tmp_array);
                $rs->MoveNext();
            }
        }
        return $list;
    }


    function Ports($limit = 10)
    {
        require_once ('classes/Port.inc');
   
        $list = array();

        $query = "SELECT count(layer4_dport) AS occurrences, layer4_dport
                  FROM acid_event 
                  GROUP BY layer4_dport
                  ORDER BY occurrences DESC LIMIT $limit;";
        if (!$rs = &$this->snort_conn->Execute($query)) {
            print $this->snort_conn->ErrorMsg();
        } else {
            while (!$rs->EOF) 
            {
                $port        = $rs->fields["layer4_dport"];
                $service     = "unknown";
                $occurrences = $rs->fields["occurrences"];

                if ($port != "NULL" and $occurrences != 0)
                {
                    if ($port_list = Port::get_list($this->ossim_conn, 
                            "WHERE port_number = $port")) 
                    {
                        $service = $port_list[0]->get_service();
                    }
                    $tmp_array = array ($port, $service, $occurrences);
                    array_push($list, $tmp_array);
                }
                $rs->MoveNext();
            }
        }
        return $list;
   }

   function AlertsByRisk($limit = 10)
   {
        $list = array();

        $query = "SELECT sig_name, ossim_risk_a
                  FROM acid_event GROUP BY sig_name
                  ORDER BY ossim_risk_a DESC LIMIT $limit;";
        if (!$rs = &$this->snort_conn->Execute($query)) {
            print $this->snort_conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                $tmp_array = array ($rs->fields["sig_name"], 
                                    $rs->fields["ossim_risk_a"]);
                array_push($list, $tmp_array);
                $rs->MoveNext();
            }
        }
        return $list;
   }
}

?>
