<?php

class RRD_conf {

    var $ip;
    var $pkt_sent;
    var $pkt_rcvd;
    var $bytes_sent;
    var $bytes_rcvd;
    var $tot_contacted_sent_peers;
    var $tot_contacted_rcvd_peers;
    var $ip_dns_sent_bytes;
    var $ip_dns_rcvd_bytes;
    var $ip_nbios_ip_sent_bytes;
    var $ip_nbios_ip_rcvd_bytes;
    var $ip_mail_sent_bytes;
    var $ip_mail_rcvd_bytes;
    var $mrtg_a;
    var $mrtg_c;

    function RRD_conf($ip, $pkt_sent, $pkt_rcvd, $bytes_sent, $bytes_rcvd,
                      $tot_contacted_sent_peers, $tot_contacted_rcvd_peers, 
                      $ip_dns_sent_bytes, $ip_dns_rcvd_bytes, 
                      $ip_nbios_ip_sent_bytes, $ip_nbios_ip_rcvd_bytes,
                      $ip_mail_sent_bytes, $ip_mail_rcvd_bytes, 
                      $mrtg_a, $mrtg_c)
    {
        $this->ip                       = $ip;
        $this->pkt_sent                 = $pkt_sent;
        $this->pkt_rcvd                 = $pkt_rcvd;
        $this->bytes_sent               = $bytes_sent;
        $this->bytes_rcvd               = $bytes_rcvd;
        $this->tot_contacted_sent_peers = $tot_contacted_sent_peers;
        $this->tot_contacted_rcvd_peers = $tot_contacted_rcvd_peers;
        $this->ip_dns_sent_bytes        = $ip_dns_sent_bytes;
        $this->ip_dns_rcvd_bytes        = $ip_dns_rcvd_bytes;
        $this->ip_nbios_ip_sent_bytes   = $ip_nbios_ip_sent_bytes;
        $this->ip_nbios_ip_rcvd_bytes   = $ip_nbios_ip_rcvd_bytes;
        $this->ip_mail_sent_bytes       = $ip_mail_sent_bytes;
        $this->ip_mail_rcvd_bytes       = $ip_mail_rcvd_bytes;
        $this->mrtg_a                   = $mrtg_a;
        $this->mrtg_c                   = $mrtg_c;
    }

    function get_col($col, $type) 
    {
        if ($col == "ip") {
            return $this->ip;
        } elseif ($col == "pkt_sent") {
            $col_val = $this->pkt_sent;
        } elseif ($col == "pkt_rcvd") {
            $col_val = $this->pkt_rcvd;
        } elseif ($col == "bytes_sent") {
            $col_val = $this->bytes_sent;
        } elseif ($col == "bytes_rcvd") {
            $col_val = $this->bytes_rcvd;
        } elseif ($col == "tot_contacted_sent_peers") {
            $col_val = $this->tot_contacted_sent_peers;
        } elseif ($col == "tot_contacted_rcvd_peers") {
            $col_val = $this->tot_contacted_rcvd_peers;
        } elseif ($col == "ip_dns_sent_bytes") {
            $col_val = $this->ip_dns_sent_bytes;
        } elseif ($col == "ip_dns_rcvd_bytes") {
            $col_val = $this->ip_dns_rcvd_bytes;
        } elseif ($col == "ip_nbios_ip_sent_bytes") {
            $col_val = $this->ip_nbios_ip_sent_bytes;
        } elseif ($col == "ip_nbios_ip_rcvd_bytes") {
            $col_val = $this->ip_nbios_ip_rcvd_bytes;
        } elseif ($col == "ip_mail_sent_bytes") {
            $col_val = $this->ip_mail_sent_bytes;
        } elseif ($col == "ip_mail_rcvd_bytes") {
            $col_val = $this->ip_mail_rcvd_bytes;
        } elseif ($col == "mrtg_a") {
            $col_val = $this->mrtg_a;
        } elseif ($col == "mrtg_c") {
            $col_val = $this->mrtg_c;
        } else {
            echo "Wrong argument at get_col\n";
            return -1;
        }
        
        list($threshold, $priority, $alpha, $beta) = split (",", $col_val, 4);
            
        if ($type == "threshold") return $threshold;
        if ($type == "priority")  return $priority;
        if ($type == "alpha")     return $alpha;
        if ($type == "beta")      return $beta;

    }

    /* 
     * Aliases 
     */
    function get_ip() {
        return $this->ip;
    }
    function get_pkt_sent_threshold() { 
        return RRD_conf::get_col("pkt_sent", "threshold");
    }
    /* 
        ...
        ...
        ...
        ¿ complete all aliases ? 
    */

    function get_list($conn, $where = "") 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM rrd_conf $where")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
            	$list[] = new RRD_conf 
                                ($rs->fields["ip"],
                                 $rs->fields["pkt_sent"],
                                 $rs->fields["pkt_rcvd"],
                                 $rs->fields["bytes_sent"],
                                 $rs->fields["bytes_rcvd"],
                                 $rs->fields["tot_contacted_sent_peers"],
                                 $rs->fields["tot_contacted_rcvd_peers"],
                                 $rs->fields["ip_dns_sent_bytes"],
                                 $rs->fields["ip_dns_rcvd_bytes"],
                                 $rs->fields["ip_nbios_ip_sent_bytes"],
                                 $rs->fields["ip_nbios_ip_rcvd_bytes"],
                                 $rs->fields["ip_mail_sent_bytes"],
                                 $rs->fields["ip_mail_rcvd_bytes"],
                                 $rs->fields["mrtg_a"],
                                 $rs->fields["mrtg_c"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function insert  ($conn, $ip, $pkt_sent, $pkt_rcvd, 
                      $bytes_sent, $bytes_rcvd,
                      $tot_contacted_sent_peers, $tot_contacted_rcvd_peers, 
                      $ip_dns_sent_bytes, $ip_dns_rcvd_bytes, 
                      $ip_nbios_ip_sent_bytes, $ip_nbios_ip_rcvd_bytes,
                      $ip_mail_sent_bytes, $ip_mail_rcvd_bytes, 
                      $mrtg_a, $mrtg_c)
    {
        $query = "INSERT INTO rrd_conf VALUES ('$ip', 
                      '$pkt_sent', '$pkt_rcvd', '$bytes_sent', '$bytes_rcvd',
                      '$tot_contacted_sent_peers', '$tot_contacted_rcvd_peers', 
                      '$ip_dns_sent_bytes', '$ip_dns_rcvd_bytes', 
                      '$ip_nbios_ip_sent_bytes', '$ip_nbios_ip_rcvd_bytes',
                      '$ip_mail_sent_bytes', '$ip_mail_rcvd_bytes', 
                      '$mrtg_a', '$mrtg_c');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function update  ($conn, $ip, $pkt_sent, $pkt_rcvd, 
                      $bytes_sent, $bytes_rcvd,
                      $tot_contacted_sent_peers, $tot_contacted_rcvd_peers, 
                      $ip_dns_sent_bytes, $ip_dns_rcvd_bytes, 
                      $ip_nbios_ip_sent_bytes, $ip_nbios_ip_rcvd_bytes,
                      $ip_mail_rcvd_bytes, $ip_mail_rcvd_bytes, 
                      $mrtg_a, $mrtg_c)
    {
        $query = "UPDATE rrd_conf SET 
                      pkt_sent = '$pkt_sent', pkt_rcvd = '$pkt_rcvd', 
                      bytes_sent = '$bytes_sent', bytes_rcvd = '$bytes_rcvd',
                      tot_contacted_sent_peers = '$tot_contacted_sent_peers', 
                      tot_contacted_rcvd_peers = '$tot_contacted_rcvd_peers', 
                      ip_dns_sent_bytes = '$ip_dns_sent_bytes', 
                      ip_dns_rcvd_bytes = '$ip_dns_rcvd_bytes', 
                      ip_nbios_ip_sent_bytes = '$ip_nbios_ip_sent_bytes', 
                      ip_nbios_ip_rcvd_bytes = '$ip_nbios_ip_rcvd_bytes',
                      ip_mail_rcvd_bytes = '$ip_mail_sent_bytes', 
                      ip_mail_rcvd_bytes = '$ip_mail_rcvd_bytes', 
                      mrtg_a = '$mrtg_a', mrtg_c = '$mrtg_c'  
                  WHERE ip = '$ip';";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

    function delete($conn, $ip) {
        $query = "DELETE FROM rrd_conf WHERE ip = '$ip';";
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

}

?>
