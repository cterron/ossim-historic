<?php

class RRD_conf_global {

    var $active_host_senders_num;
    var $arp_rarp_bytes;
    var $broadcast_pkts;
    var $ethernet_bytes;
    var $ethernet_pkts;
    var $icmp_bytes;
    var $igmp_bytes;
    var $ip_bytes;
    var $ip_dhcp_bootp_bytes;
    var $ip_dns_bytes;
    var $ip_edonkey_bytes;
    var $ip_ftp_bytes;
    var $ip_gnutella_bytes;
    var $ip_http_bytes;
    var $ip_kazaa_bytes;
    var $ip_mail_bytes;
    var $ip_messenger_bytes;
    var $ip_nbios_ip_bytes;
    var $ip_nfs_bytes;
    var $ip_nttp_bytes;
    var $ip_snmp_bytes;
    var $ip_ssh_bytes;
    var $ip_telnet_bytes;
    var $ip_winmx_bytes;
    var $ip_x11_bytes;
    var $ipx_bytes;
    var $known_hosts_num;
    var $multicast_pkts;
    var $ospf_bytes;
    var $other_bytes;
    var $tcp_bytes;
    var $udp_bytes;
    var $up_to_1024_pkts;
    var $up_to_128_pkts;
    var $up_to_1518_pkts;
    var $up_to_512_pkts;
    var $up_to_64_pkts;

/*          For use with regexps

$active_host_senders_num;
$arp_rarp_bytes;
$broadcast_pkts;
$ethernet_bytes;
$ethernet_pkts;
$icmp_bytes;
$igmp_bytes;
$ip_bytes;
$ip_dhcp_bootp_bytes;
$ip_dns_bytes;
$ip_edonkey_bytes;
$ip_ftp_bytes;
$ip_gnutella_bytes;
$ip_http_bytes;
$ip_kazaa_bytes;
$ip_mail_bytes;
$ip_messenger_bytes;
$ip_nbios_ip_bytes;
$ip_nfs_bytes;
$ip_nttp_bytes;
$ip_snmp_bytes;
$ip_ssh_bytes;
$ip_telnet_bytes;
$ip_winmx_bytes;
$ip_x11_bytes;
$ipx_bytes;
$known_hosts_num;
$multicast_pkts;
$ospf_bytes;
$other_bytes;
$tcp_bytes;
$udp_bytes;
$up_to_1024_pkts;
$up_to_128_pkts;
$up_to_1518_pkts;
$up_to_512_pkts;
$up_to_64_pkts;
*/


function RRD_conf_global($active_host_senders_num, $arp_rarp_bytes,
$broadcast_pkts, $ethernet_bytes, $ethernet_pkts, $icmp_bytes, $igmp_bytes,
$ip_bytes, $ip_dhcp_bootp_bytes, $ip_dns_bytes, $ip_edonkey_bytes,
$ip_ftp_bytes, $ip_gnutella_bytes, $ip_http_bytes, $ip_kazaa_bytes,
$ip_mail_bytes, $ip_messenger_bytes, $ip_nbios_ip_bytes, $ip_nfs_bytes,
$ip_nttp_bytes, $ip_snmp_bytes, $ip_ssh_bytes, $ip_telnet_bytes,
$ip_winmx_bytes, $ip_x11_bytes, $ipx_bytes, $known_hosts_num, $multicast_pkts,
$ospf_bytes, $other_bytes, $tcp_bytes, $udp_bytes, $up_to_1024_pkts,
$up_to_128_pkts, $up_to_1518_pkts, $up_to_512_pkts, $up_to_64_pkts) {


    $this->active_host_senders_num = $active_host_senders_num;
    $this->arp_rarp_bytes = $arp_rarp_bytes;
    $this->broadcast_pkts = $broadcast_pkts;
    $this->ethernet_bytes = $ethernet_bytes;
    $this->ethernet_pkts = $ethernet_pkts;
    $this->icmp_bytes = $icmp_bytes;
    $this->igmp_bytes = $igmp_bytes;
    $this->ip_bytes = $ip_bytes;
    $this->ip_dhcp_bootp_bytes = $ip_dhcp_bootp_bytes;
    $this->ip_dns_bytes = $ip_dns_bytes;
    $this->ip_edonkey_bytes = $ip_edonkey_bytes;
    $this->ip_ftp_bytes = $ip_ftp_bytes;
    $this->ip_gnutella_bytes = $ip_gnutella_bytes;
    $this->ip_http_bytes = $ip_http_bytes;
    $this->ip_kazaa_bytes = $ip_kazaa_bytes;
    $this->ip_mail_bytes = $ip_mail_bytes;
    $this->ip_messenger_bytes = $ip_messenger_bytes;
    $this->ip_nbios_ip_bytes = $ip_nbios_ip_bytes;
    $this->ip_nfs_bytes = $ip_nfs_bytes;
    $this->ip_nttp_bytes = $ip_nttp_bytes;
    $this->ip_snmp_bytes = $ip_snmp_bytes;
    $this->ip_ssh_bytes = $ip_ssh_bytes;
    $this->ip_telnet_bytes = $ip_telnet_bytes;
    $this->ip_winmx_bytes = $ip_winmx_bytes;
    $this->ip_x11_bytes = $ip_x11_bytes;
    $this->ipx_bytes = $ipx_bytes;
    $this->known_hosts_num = $known_hosts_num;
    $this->multicast_pkts = $multicast_pkts;
    $this->ospf_bytes = $ospf_bytes;
    $this->other_bytes = $other_bytes;
    $this->tcp_bytes = $tcp_bytes;
    $this->udp_bytes = $udp_bytes;
    $this->up_to_1024_pkts = $up_to_1024_pkts;
    $this->up_to_128_pkts = $up_to_128_pkts;
    $this->up_to_1518_pkts = $up_to_1518_pkts;
    $this->up_to_512_pkts = $up_to_512_pkts;
    $this->up_to_64_pkts = $up_to_64_pkts;

    }

    function get_col($col, $type) 
    {
if ($col == "active_host_senders_num") { $col_val = $this->active_host_senders_num;
} elseif ($col == "arp_rarp_bytes") { $col_val = $this->arp_rarp_bytes;
} elseif ($col == "broadcast_pkts") { $col_val = $this->broadcast_pkts;
} elseif ($col == "ethernet_bytes") { $col_val = $this->ethernet_bytes;
} elseif ($col == "ethernet_pkts") { $col_val = $this->ethernet_pkts;
} elseif ($col == "icmp_bytes") { $col_val = $this->icmp_bytes;
} elseif ($col == "igmp_bytes") { $col_val = $this->igmp_bytes;
} elseif ($col == "ip_bytes") { $col_val = $this->ip_bytes;
} elseif ($col == "ip_dhcp_bootp_bytes") { $col_val = $this->ip_dhcp_bootp_bytes;
} elseif ($col == "ip_dns_bytes") { $col_val = $this->ip_dns_bytes;
} elseif ($col == "ip_edonkey_bytes") { $col_val = $this->ip_edonkey_bytes;
} elseif ($col == "ip_ftp_bytes") { $col_val = $this->ip_ftp_bytes;
} elseif ($col == "ip_gnutella_bytes") { $col_val = $this->ip_gnutella_bytes;
} elseif ($col == "ip_http_bytes") { $col_val = $this->ip_http_bytes;
} elseif ($col == "ip_kazaa_bytes") { $col_val = $this->ip_kazaa_bytes;
} elseif ($col == "ip_mail_bytes") { $col_val = $this->ip_mail_bytes;
} elseif ($col == "ip_messenger_bytes") { $col_val = $this->ip_messenger_bytes;
} elseif ($col == "ip_nbios_ip_bytes") { $col_val = $this->ip_nbios_ip_bytes;
} elseif ($col == "ip_nfs_bytes") { $col_val = $this->ip_nfs_bytes;
} elseif ($col == "ip_nttp_bytes") { $col_val = $this->ip_nttp_bytes;
} elseif ($col == "ip_snmp_bytes") { $col_val = $this->ip_snmp_bytes;
} elseif ($col == "ip_ssh_bytes") { $col_val = $this->ip_ssh_bytes;
} elseif ($col == "ip_telnet_bytes") { $col_val = $this->ip_telnet_bytes;
} elseif ($col == "ip_winmx_bytes") { $col_val = $this->ip_winmx_bytes;
} elseif ($col == "ip_x11_bytes") { $col_val = $this->ip_x11_bytes;
} elseif ($col == "ipx_bytes") { $col_val = $this->ipx_bytes;
} elseif ($col == "known_hosts_num") { $col_val = $this->known_hosts_num;
} elseif ($col == "multicast_pkts") { $col_val = $this->multicast_pkts;
} elseif ($col == "ospf_bytes") { $col_val = $this->ospf_bytes;
} elseif ($col == "other_bytes") { $col_val = $this->other_bytes;
} elseif ($col == "tcp_bytes") { $col_val = $this->tcp_bytes;
} elseif ($col == "udp_bytes") { $col_val = $this->udp_bytes;
} elseif ($col == "up_to_1024_pkts") { $col_val = $this->up_to_1024_pkts;
} elseif ($col == "up_to_128_pkts") { $col_val = $this->up_to_128_pkts;
} elseif ($col == "up_to_1518_pkts") { $col_val = $this->up_to_1518_pkts;
} elseif ($col == "up_to_512_pkts") { $col_val = $this->up_to_512_pkts;
} elseif ($col == "up_to_64_pkts") { $col_val = $this->up_to_64_pkts;
        } else {
            echo "Wrong argument at get_col\n";
            return -1;
        }
        
        list($threshold, $priority, $alpha, $beta, $persistence) = split (",", $col_val, 5);
            
        if ($type == "threshold") return $threshold;
        if ($type == "priority")  return $priority;
        if ($type == "alpha")     return $alpha;
        if ($type == "beta")      return $beta;
        if ($type == "persistence")      return $persistence;

    }

    function get_list($conn, $where = "") 
    {
        if (!$rs = &$conn->Execute("SELECT * FROM rrd_conf_global $where")) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
            	$list[] = new RRD_conf_global( 
$rs->fields["active_host_senders_num"],
$rs->fields["arp_rarp_bytes"],
$rs->fields["broadcast_pkts"],
$rs->fields["ethernet_bytes"],
$rs->fields["ethernet_pkts"],
$rs->fields["icmp_bytes"],
$rs->fields["igmp_bytes"],
$rs->fields["ip_bytes"],
$rs->fields["ip_dhcp_bootp_bytes"],
$rs->fields["ip_dns_bytes"],
$rs->fields["ip_edonkey_bytes"],
$rs->fields["ip_ftp_bytes"],
$rs->fields["ip_gnutella_bytes"],
$rs->fields["ip_http_bytes"],
$rs->fields["ip_kazaa_bytes"],
$rs->fields["ip_mail_bytes"],
$rs->fields["ip_messenger_bytes"],
$rs->fields["ip_nbios_ip_bytes"],
$rs->fields["ip_nfs_bytes"],
$rs->fields["ip_nttp_bytes"],
$rs->fields["ip_snmp_bytes"],
$rs->fields["ip_ssh_bytes"],
$rs->fields["ip_telnet_bytes"],
$rs->fields["ip_winmx_bytes"],
$rs->fields["ip_x11_bytes"],
$rs->fields["ipx_bytes"],
$rs->fields["known_hosts_num"],
$rs->fields["multicast_pkts"],
$rs->fields["ospf_bytes"],
$rs->fields["other_bytes"],
$rs->fields["tcp_bytes"],
$rs->fields["udp_bytes"],
$rs->fields["up_to_1024_pkts"],
$rs->fields["up_to_128_pkts"],
$rs->fields["up_to_1518_pkts"],
$rs->fields["up_to_512_pkts"],
$rs->fields["up_to_64_pkts"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }
function insert ($conn, $active_host_senders_num, $arp_rarp_bytes,
$broadcast_pkts, $ethernet_bytes, $ethernet_pkts, $icmp_bytes, $igmp_bytes,
$ip_bytes, $ip_dhcp_bootp_bytes, $ip_dns_bytes, $ip_edonkey_bytes,
$ip_ftp_bytes, $ip_gnutella_bytes, $ip_http_bytes, $ip_kazaa_bytes,
$ip_mail_bytes, $ip_messenger_bytes, $ip_nbios_ip_bytes, $ip_nfs_bytes,
$ip_nttp_bytes, $ip_snmp_bytes, $ip_ssh_bytes, $ip_telnet_bytes,
$ip_winmx_bytes, $ip_x11_bytes, $ipx_bytes, $known_hosts_num, $multicast_pkts,
$ospf_bytes, $other_bytes, $tcp_bytes, $udp_bytes, $up_to_1024_pkts,
$up_to_128_pkts, $up_to_1518_pkts, $up_to_512_pkts, $up_to_64_pkts) {

        $query = "INSERT INTO rrd_conf_global VALUES ( '$active_host_senders_num', '$arp_rarp_bytes', '$broadcast_pkts', '$ethernet_bytes', '$ethernet_pkts', '$icmp_bytes', '$igmp_bytes', '$ip_bytes', '$ip_dhcp_bootp_bytes', '$ip_dns_bytes', '$ip_edonkey_bytes', '$ip_ftp_bytes', '$ip_gnutella_bytes', '$ip_http_bytes', '$ip_kazaa_bytes', '$ip_mail_bytes', '$ip_messenger_bytes', '$ip_nbios_ip_bytes', '$ip_nfs_bytes', '$ip_nttp_bytes', '$ip_snmp_bytes', '$ip_ssh_bytes', '$ip_telnet_bytes', '$ip_winmx_bytes', '$ip_x11_bytes', '$ipx_bytes', '$known_hosts_num', '$multicast_pkts', '$ospf_bytes', '$other_bytes', '$tcp_bytes', '$udp_bytes', '$up_to_1024_pkts', '$up_to_128_pkts', '$up_to_1518_pkts', '$up_to_512_pkts', '$up_to_64_pkts');";
        if ($conn->Execute($query) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }


function update ($conn, $active_host_senders_num, $arp_rarp_bytes,
$broadcast_pkts, $ethernet_bytes, $ethernet_pkts, $icmp_bytes, $igmp_bytes,
$ip_bytes, $ip_dhcp_bootp_bytes, $ip_dns_bytes, $ip_edonkey_bytes,
$ip_ftp_bytes, $ip_gnutella_bytes, $ip_http_bytes, $ip_kazaa_bytes,
$ip_mail_bytes, $ip_messenger_bytes, $ip_nbios_ip_bytes, $ip_nfs_bytes,
$ip_nttp_bytes, $ip_snmp_bytes, $ip_ssh_bytes, $ip_telnet_bytes,
$ip_winmx_bytes, $ip_x11_bytes, $ipx_bytes, $known_hosts_num, $multicast_pkts,
$ospf_bytes, $other_bytes, $tcp_bytes, $udp_bytes, $up_to_1024_pkts,
$up_to_128_pkts, $up_to_1518_pkts, $up_to_512_pkts, $up_to_64_pkts) {

        $query = "UPDATE rrd_conf_global SET active_host_senders_num = '$active_host_senders_num', arp_rarp_bytes = '$arp_rarp_bytes', broadcast_pkts = '$broadcast_pkts', ethernet_bytes = '$ethernet_bytes', ethernet_pkts = '$ethernet_pkts', icmp_bytes = '$icmp_bytes', igmp_bytes = '$igmp_bytes', ip_bytes = '$ip_bytes', ip_dhcp_bootp_bytes = '$ip_dhcp_bootp_bytes', ip_dns_bytes = '$ip_dns_bytes', ip_edonkey_bytes = '$ip_edonkey_bytes', ip_ftp_bytes = '$ip_ftp_bytes', ip_gnutella_bytes = '$ip_gnutella_bytes', ip_http_bytes = '$ip_http_bytes', ip_kazaa_bytes = '$ip_kazaa_bytes', ip_mail_bytes = '$ip_mail_bytes', ip_messenger_bytes = '$ip_messenger_bytes', ip_nbios_ip_bytes = '$ip_nbios_ip_bytes', ip_nfs_bytes = '$ip_nfs_bytes', ip_nttp_bytes = '$ip_nttp_bytes', ip_snmp_bytes = '$ip_snmp_bytes', ip_ssh_bytes = '$ip_ssh_bytes', ip_telnet_bytes = '$ip_telnet_bytes', ip_winmx_bytes = '$ip_winmx_bytes', ip_x11_bytes = '$ip_x11_bytes', ipx_bytes = '$ipx_bytes', known_hosts_num = '$known_hosts_num', multicast_pkts = '$multicast_pkts', ospf_bytes = '$ospf_bytes', other_bytes = '$other_bytes', tcp_bytes = '$tcp_bytes', udp_bytes = '$udp_bytes', up_to_1024_pkts = '$up_to_1024_pkts', up_to_128_pkts = '$up_to_128_pkts', up_to_1518_pkts = '$up_to_1518_pkts', up_to_512_pkts = '$up_to_512_pkts', up_to_64_pkts = '$up_to_64_pkts';";
        if ($conn->Execute($query) === false) {
            print 'error updating: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

}

?>
<?php


