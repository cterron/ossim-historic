<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Modify RRD config</h1>

<?php

    /* check params */
    if ((mysql_escape_string($_POST["insert"])) &&
        (!mysql_escape_string($_POST["ip"]))) 
    {
?>

  <p align="center">Please, complete all the fields</p>
  <?php exit();?>

<?php

/* check OK, insert into BD */
} elseif(mysql_escape_string($_POST["insert"])) {

    $ip = mysql_escape_string($_POST["ip"]);

    if($ip == "global") {

	$active_host_senders_num
 		= implode(",", array(mysql_escape_string($_POST["active_host_senders_num_threshold"]),
			 mysql_escape_string($_POST["active_host_senders_num_priority"]),
			 mysql_escape_string($_POST["active_host_senders_num_alpha"]),
			 mysql_escape_string($_POST["active_host_senders_num_beta"]),
			 mysql_escape_string($_POST["active_host_senders_num_persistence"])));
	$arp_rarp_bytes
 		= implode(",", array(mysql_escape_string($_POST["arp_rarp_bytes_threshold"]),
			 mysql_escape_string($_POST["arp_rarp_bytes_priority"]),
			 mysql_escape_string($_POST["arp_rarp_bytes_alpha"]),
			 mysql_escape_string($_POST["arp_rarp_bytes_beta"]),
			 mysql_escape_string($_POST["arp_rarp_bytes_persistence"])));
	$broadcast_pkts
 		= implode(",", array(mysql_escape_string($_POST["broadcast_pkts_threshold"]),
			 mysql_escape_string($_POST["broadcast_pkts_priority"]),
			 mysql_escape_string($_POST["broadcast_pkts_alpha"]),
			 mysql_escape_string($_POST["broadcast_pkts_beta"]),
			 mysql_escape_string($_POST["broadcast_pkts_persistence"])));
	$ethernet_bytes
 		= implode(",", array(mysql_escape_string($_POST["ethernet_bytes_threshold"]),
			 mysql_escape_string($_POST["ethernet_bytes_priority"]),
			 mysql_escape_string($_POST["ethernet_bytes_alpha"]),
			 mysql_escape_string($_POST["ethernet_bytes_beta"]),
			 mysql_escape_string($_POST["ethernet_bytes_persistence"])));
	$ethernet_pkts
 		= implode(",", array(mysql_escape_string($_POST["ethernet_pkts_threshold"]),
			 mysql_escape_string($_POST["ethernet_pkts_priority"]),
			 mysql_escape_string($_POST["ethernet_pkts_alpha"]),
			 mysql_escape_string($_POST["ethernet_pkts_beta"]),
			 mysql_escape_string($_POST["ethernet_pkts_persistence"])));
	$icmp_bytes
 		= implode(",", array(mysql_escape_string($_POST["icmp_bytes_threshold"]),
			 mysql_escape_string($_POST["icmp_bytes_priority"]),
			 mysql_escape_string($_POST["icmp_bytes_alpha"]),
			 mysql_escape_string($_POST["icmp_bytes_beta"]),
			 mysql_escape_string($_POST["icmp_bytes_persistence"])));
	$igmp_bytes
 		= implode(",", array(mysql_escape_string($_POST["igmp_bytes_threshold"]),
			 mysql_escape_string($_POST["igmp_bytes_priority"]),
			 mysql_escape_string($_POST["igmp_bytes_alpha"]),
			 mysql_escape_string($_POST["igmp_bytes_beta"]),
			 mysql_escape_string($_POST["igmp_bytes_persistence"])));
	$ip_bytes
 		= implode(",", array(mysql_escape_string($_POST["ip_bytes_threshold"]),
			 mysql_escape_string($_POST["ip_bytes_priority"]),
			 mysql_escape_string($_POST["ip_bytes_alpha"]),
			 mysql_escape_string($_POST["ip_bytes_beta"]),
			 mysql_escape_string($_POST["ip_bytes_persistence"])));
	$ip_dhcp_bootp_bytes
 		= implode(",", array(mysql_escape_string($_POST["ip_dhcp_bootp_bytes_threshold"]),
			 mysql_escape_string($_POST["ip_dhcp_bootp_bytes_priority"]),
			 mysql_escape_string($_POST["ip_dhcp_bootp_bytes_alpha"]),
			 mysql_escape_string($_POST["ip_dhcp_bootp_bytes_beta"]),
			 mysql_escape_string($_POST["ip_dhcp_bootp_bytes_persistence"])));
	$ip_dns_bytes
 		= implode(",", array(mysql_escape_string($_POST["ip_dns_bytes_threshold"]),
			 mysql_escape_string($_POST["ip_dns_bytes_priority"]),
			 mysql_escape_string($_POST["ip_dns_bytes_alpha"]),
			 mysql_escape_string($_POST["ip_dns_bytes_beta"]),
			 mysql_escape_string($_POST["ip_dns_bytes_persistence"])));
	$ip_edonkey_bytes
 		= implode(",", array(mysql_escape_string($_POST["ip_edonkey_bytes_threshold"]),
			 mysql_escape_string($_POST["ip_edonkey_bytes_priority"]),
			 mysql_escape_string($_POST["ip_edonkey_bytes_alpha"]),
			 mysql_escape_string($_POST["ip_edonkey_bytes_beta"]),
			 mysql_escape_string($_POST["ip_edonkey_bytes_persistence"])));
	$ip_ftp_bytes
 		= implode(",", array(mysql_escape_string($_POST["ip_ftp_bytes_threshold"]),
			 mysql_escape_string($_POST["ip_ftp_bytes_priority"]),
			 mysql_escape_string($_POST["ip_ftp_bytes_alpha"]),
			 mysql_escape_string($_POST["ip_ftp_bytes_beta"]),
			 mysql_escape_string($_POST["ip_ftp_bytes_persistence"])));
	$ip_gnutella_bytes
 		= implode(",", array(mysql_escape_string($_POST["ip_gnutella_bytes_threshold"]),
			 mysql_escape_string($_POST["ip_gnutella_bytes_priority"]),
			 mysql_escape_string($_POST["ip_gnutella_bytes_alpha"]),
			 mysql_escape_string($_POST["ip_gnutella_bytes_beta"]),
			 mysql_escape_string($_POST["ip_gnutella_bytes_persistence"])));
	$ip_http_bytes
 		= implode(",", array(mysql_escape_string($_POST["ip_http_bytes_threshold"]),
			 mysql_escape_string($_POST["ip_http_bytes_priority"]),
			 mysql_escape_string($_POST["ip_http_bytes_alpha"]),
			 mysql_escape_string($_POST["ip_http_bytes_beta"]),
			 mysql_escape_string($_POST["ip_http_bytes_persistence"])));
	$ip_kazaa_bytes
 		= implode(",", array(mysql_escape_string($_POST["ip_kazaa_bytes_threshold"]),
			 mysql_escape_string($_POST["ip_kazaa_bytes_priority"]),
			 mysql_escape_string($_POST["ip_kazaa_bytes_alpha"]),
			 mysql_escape_string($_POST["ip_kazaa_bytes_beta"]),
			 mysql_escape_string($_POST["ip_kazaa_bytes_persistence"])));
	$ip_mail_bytes
 		= implode(",", array(mysql_escape_string($_POST["ip_mail_bytes_threshold"]),
			 mysql_escape_string($_POST["ip_mail_bytes_priority"]),
			 mysql_escape_string($_POST["ip_mail_bytes_alpha"]),
			 mysql_escape_string($_POST["ip_mail_bytes_beta"]),
			 mysql_escape_string($_POST["ip_mail_bytes_persistence"])));
	$ip_messenger_bytes
 		= implode(",", array(mysql_escape_string($_POST["ip_messenger_bytes_threshold"]),
			 mysql_escape_string($_POST["ip_messenger_bytes_priority"]),
			 mysql_escape_string($_POST["ip_messenger_bytes_alpha"]),
			 mysql_escape_string($_POST["ip_messenger_bytes_beta"]),
			 mysql_escape_string($_POST["ip_messenger_bytes_persistence"])));
	$ip_nbios_ip_bytes
 		= implode(",", array(mysql_escape_string($_POST["ip_nbios_ip_bytes_threshold"]),
			 mysql_escape_string($_POST["ip_nbios_ip_bytes_priority"]),
			 mysql_escape_string($_POST["ip_nbios_ip_bytes_alpha"]),
			 mysql_escape_string($_POST["ip_nbios_ip_bytes_beta"]),
			 mysql_escape_string($_POST["ip_nbios_ip_bytes_persistence"])));
	$ip_nfs_bytes
 		= implode(",", array(mysql_escape_string($_POST["ip_nfs_bytes_threshold"]),
			 mysql_escape_string($_POST["ip_nfs_bytes_priority"]),
			 mysql_escape_string($_POST["ip_nfs_bytes_alpha"]),
			 mysql_escape_string($_POST["ip_nfs_bytes_beta"]),
			 mysql_escape_string($_POST["ip_nfs_bytes_persistence"])));
	$ip_nttp_bytes
 		= implode(",", array(mysql_escape_string($_POST["ip_nttp_bytes_threshold"]),
			 mysql_escape_string($_POST["ip_nttp_bytes_priority"]),
			 mysql_escape_string($_POST["ip_nttp_bytes_alpha"]),
			 mysql_escape_string($_POST["ip_nttp_bytes_beta"]),
			 mysql_escape_string($_POST["ip_nttp_bytes_persistence"])));
	$ip_snmp_bytes
 		= implode(",", array(mysql_escape_string($_POST["ip_snmp_bytes_threshold"]),
			 mysql_escape_string($_POST["ip_snmp_bytes_priority"]),
			 mysql_escape_string($_POST["ip_snmp_bytes_alpha"]),
			 mysql_escape_string($_POST["ip_snmp_bytes_beta"]),
			 mysql_escape_string($_POST["ip_snmp_bytes_persistence"])));
	$ip_ssh_bytes
 		= implode(",", array(mysql_escape_string($_POST["ip_ssh_bytes_threshold"]),
			 mysql_escape_string($_POST["ip_ssh_bytes_priority"]),
			 mysql_escape_string($_POST["ip_ssh_bytes_alpha"]),
			 mysql_escape_string($_POST["ip_ssh_bytes_beta"]),
			 mysql_escape_string($_POST["ip_ssh_bytes_persistence"])));
	$ip_telnet_bytes
 		= implode(",", array(mysql_escape_string($_POST["ip_telnet_bytes_threshold"]),
			 mysql_escape_string($_POST["ip_telnet_bytes_priority"]),
			 mysql_escape_string($_POST["ip_telnet_bytes_alpha"]),
			 mysql_escape_string($_POST["ip_telnet_bytes_beta"]),
			 mysql_escape_string($_POST["ip_telnet_bytes_persistence"])));
	$ip_winmx_bytes
 		= implode(",", array(mysql_escape_string($_POST["ip_winmx_bytes_threshold"]),
			 mysql_escape_string($_POST["ip_winmx_bytes_priority"]),
			 mysql_escape_string($_POST["ip_winmx_bytes_alpha"]),
			 mysql_escape_string($_POST["ip_winmx_bytes_beta"]),
			 mysql_escape_string($_POST["ip_winmx_bytes_persistence"])));
	$ip_x11_bytes
 		= implode(",", array(mysql_escape_string($_POST["ip_x11_bytes_threshold"]),
			 mysql_escape_string($_POST["ip_x11_bytes_priority"]),
			 mysql_escape_string($_POST["ip_x11_bytes_alpha"]),
			 mysql_escape_string($_POST["ip_x11_bytes_beta"]),
			 mysql_escape_string($_POST["ip_x11_bytes_persistence"])));
	$ipx_bytes
 		= implode(",", array(mysql_escape_string($_POST["ipx_bytes_threshold"]),
			 mysql_escape_string($_POST["ipx_bytes_priority"]),
			 mysql_escape_string($_POST["ipx_bytes_alpha"]),
			 mysql_escape_string($_POST["ipx_bytes_beta"]),
			 mysql_escape_string($_POST["ipx_bytes_persistence"])));
	$known_hosts_num
 		= implode(",", array(mysql_escape_string($_POST["known_hosts_num_threshold"]),
			 mysql_escape_string($_POST["known_hosts_num_priority"]),
			 mysql_escape_string($_POST["known_hosts_num_alpha"]),
			 mysql_escape_string($_POST["known_hosts_num_beta"]),
			 mysql_escape_string($_POST["known_hosts_num_persistence"])));
	$multicast_pkts
 		= implode(",", array(mysql_escape_string($_POST["multicast_pkts_threshold"]),
			 mysql_escape_string($_POST["multicast_pkts_priority"]),
			 mysql_escape_string($_POST["multicast_pkts_alpha"]),
			 mysql_escape_string($_POST["multicast_pkts_beta"]),
			 mysql_escape_string($_POST["multicast_pkts_persistence"])));
	$ospf_bytes
 		= implode(",", array(mysql_escape_string($_POST["ospf_bytes_threshold"]),
			 mysql_escape_string($_POST["ospf_bytes_priority"]),
			 mysql_escape_string($_POST["ospf_bytes_alpha"]),
			 mysql_escape_string($_POST["ospf_bytes_beta"]),
			 mysql_escape_string($_POST["ospf_bytes_persistence"])));
	$other_bytes
 		= implode(",", array(mysql_escape_string($_POST["other_bytes_threshold"]),
			 mysql_escape_string($_POST["other_bytes_priority"]),
			 mysql_escape_string($_POST["other_bytes_alpha"]),
			 mysql_escape_string($_POST["other_bytes_beta"]),
			 mysql_escape_string($_POST["other_bytes_persistence"])));
	$tcp_bytes
 		= implode(",", array(mysql_escape_string($_POST["tcp_bytes_threshold"]),
			 mysql_escape_string($_POST["tcp_bytes_priority"]),
			 mysql_escape_string($_POST["tcp_bytes_alpha"]),
			 mysql_escape_string($_POST["tcp_bytes_beta"]),
			 mysql_escape_string($_POST["tcp_bytes_persistence"])));
	$udp_bytes
 		= implode(",", array(mysql_escape_string($_POST["udp_bytes_threshold"]),
			 mysql_escape_string($_POST["udp_bytes_priority"]),
			 mysql_escape_string($_POST["udp_bytes_alpha"]),
			 mysql_escape_string($_POST["udp_bytes_beta"]),
			 mysql_escape_string($_POST["udp_bytes_persistence"])));
	$up_to_1024_pkts
 		= implode(",", array(mysql_escape_string($_POST["up_to_1024_pkts_threshold"]),
			 mysql_escape_string($_POST["up_to_1024_pkts_priority"]),
			 mysql_escape_string($_POST["up_to_1024_pkts_alpha"]),
			 mysql_escape_string($_POST["up_to_1024_pkts_beta"]),
			 mysql_escape_string($_POST["up_to_1024_pkts_persistence"])));
	$up_to_128_pkts
 		= implode(",", array(mysql_escape_string($_POST["up_to_128_pkts_threshold"]),
			 mysql_escape_string($_POST["up_to_128_pkts_priority"]),
			 mysql_escape_string($_POST["up_to_128_pkts_alpha"]),
			 mysql_escape_string($_POST["up_to_128_pkts_beta"]),
			 mysql_escape_string($_POST["up_to_128_pkts_persistence"])));
	$up_to_1518_pkts
 		= implode(",", array(mysql_escape_string($_POST["up_to_1518_pkts_threshold"]),
			 mysql_escape_string($_POST["up_to_1518_pkts_priority"]),
			 mysql_escape_string($_POST["up_to_1518_pkts_alpha"]),
			 mysql_escape_string($_POST["up_to_1518_pkts_beta"]),
			 mysql_escape_string($_POST["up_to_1518_pkts_persistence"])));
	$up_to_512_pkts
 		= implode(",", array(mysql_escape_string($_POST["up_to_512_pkts_threshold"]),
			 mysql_escape_string($_POST["up_to_512_pkts_priority"]),
			 mysql_escape_string($_POST["up_to_512_pkts_alpha"]),
			 mysql_escape_string($_POST["up_to_512_pkts_beta"]),
			 mysql_escape_string($_POST["up_to_512_pkts_persistence"])));
	$up_to_64_pkts
 		= implode(",", array(mysql_escape_string($_POST["up_to_64_pkts_threshold"]),
			 mysql_escape_string($_POST["up_to_64_pkts_priority"]),
			 mysql_escape_string($_POST["up_to_64_pkts_alpha"]),
			 mysql_escape_string($_POST["up_to_64_pkts_beta"]),
			 mysql_escape_string($_POST["up_to_64_pkts_persistence"])));

    require_once 'ossim_db.inc';
    require_once 'classes/RRD_conf_global.inc';
    $db = new ossim_db();
    $conn = $db->connect();

    RRD_conf_global::update ($conn, $active_host_senders_num, $arp_rarp_bytes,
    $broadcast_pkts, $ethernet_bytes, $ethernet_pkts, $icmp_bytes, $igmp_bytes,
    $ip_bytes, $ip_dhcp_bootp_bytes, $ip_dns_bytes, $ip_edonkey_bytes,
    $ip_ftp_bytes, $ip_gnutella_bytes, $ip_http_bytes, $ip_kazaa_bytes,
    $ip_mail_bytes, $ip_messenger_bytes, $ip_nbios_ip_bytes, $ip_nfs_bytes,
    $ip_nttp_bytes, $ip_snmp_bytes, $ip_ssh_bytes, $ip_telnet_bytes,
    $ip_winmx_bytes, $ip_x11_bytes, $ipx_bytes, $known_hosts_num,
    $multicast_pkts, $ospf_bytes, $other_bytes, $tcp_bytes, $udp_bytes,
    $up_to_1024_pkts, $up_to_128_pkts, $up_to_1518_pkts, $up_to_512_pkts,
    $up_to_64_pkts);

    } else {

    $pkt_sent 
        = implode(",", array(mysql_escape_string($_POST["pkt_sent_threshold"]),
                             mysql_escape_string($_POST["pkt_sent_priority"]),
                             mysql_escape_string($_POST["pkt_sent_alpha"]),
                             mysql_escape_string($_POST["pkt_sent_beta"]),
                             mysql_escape_string($_POST["pkt_sent_persistence"])));
    $pkt_rcvd
        = implode(",", array(mysql_escape_string($_POST["pkt_rcvd_threshold"]),
                             mysql_escape_string($_POST["pkt_rcvd_priority"]),
                             mysql_escape_string($_POST["pkt_rcvd_alpha"]),
                             mysql_escape_string($_POST["pkt_rcvd_beta"]),
                             mysql_escape_string($_POST["pkt_rcvd_persistence"])));
    $bytes_sent
        = implode(",", array(mysql_escape_string($_POST["bytes_sent_threshold"]),
                             mysql_escape_string($_POST["bytes_sent_priority"]),
                             mysql_escape_string($_POST["bytes_sent_alpha"]),
                             mysql_escape_string($_POST["bytes_sent_beta"]),
                             mysql_escape_string($_POST["bytes_sent_persistence"])));
    $bytes_rcvd
        = implode(",", array(mysql_escape_string($_POST["bytes_rcvd_threshold"]),
                             mysql_escape_string($_POST["bytes_rcvd_priority"]),
                             mysql_escape_string($_POST["bytes_rcvd_alpha"]),
                             mysql_escape_string($_POST["bytes_rcvd_beta"]),
                             mysql_escape_string($_POST["bytes_rcvd_persistence"])));
    $tot_contacted_sent_peers
        = implode(",", array(mysql_escape_string($_POST["tot_contacted_sent_peers_threshold"]),
                             mysql_escape_string($_POST["tot_contacted_sent_peers_priority"]),
                             mysql_escape_string($_POST["tot_contacted_sent_peers_alpha"]),
                             mysql_escape_string($_POST["tot_contacted_sent_peers_beta"]),
                             mysql_escape_string($_POST["tot_contacted_sent_peers_persistence"])));
    $tot_contacted_rcvd_peers
        = implode(",", array(mysql_escape_string($_POST["tot_contacted_rcvd_peers_threshold"]),
                             mysql_escape_string($_POST["tot_contacted_rcvd_peers_priority"]),
                             mysql_escape_string($_POST["tot_contacted_rcvd_peers_alpha"]),
                             mysql_escape_string($_POST["tot_contacted_rcvd_peers_beta"]),
                             mysql_escape_string($_POST["tot_contacted_rcvd_peers_persistence"])));
    $ip_dns_sent_bytes
        = implode(",", array(mysql_escape_string($_POST["ip_dns_sent_bytes_threshold"]),
                             mysql_escape_string($_POST["ip_dns_sent_bytes_priority"]),
                             mysql_escape_string($_POST["ip_dns_sent_bytes_alpha"]),
                             mysql_escape_string($_POST["ip_dns_sent_bytes_beta"]),
                             mysql_escape_string($_POST["ip_dns_sent_bytes_persistence"])));
    $ip_dns_rcvd_bytes
        = implode(",", array(mysql_escape_string($_POST["ip_dns_rcvd_bytes_threshold"]),
                             mysql_escape_string($_POST["ip_dns_rcvd_bytes_priority"]),
                             mysql_escape_string($_POST["ip_dns_rcvd_bytes_alpha"]),
                             mysql_escape_string($_POST["ip_dns_rcvd_bytes_beta"]),
                             mysql_escape_string($_POST["ip_dns_rcvd_bytes_persistence"])));
    $ip_nbios_ip_sent_bytes
        = implode(",", array(mysql_escape_string($_POST["ip_nbios_ip_sent_bytes_threshold"]),
                             mysql_escape_string($_POST["ip_nbios_ip_sent_bytes_priority"]),
                             mysql_escape_string($_POST["ip_nbios_ip_sent_bytes_alpha"]),
                             mysql_escape_string($_POST["ip_nbios_ip_sent_bytes_beta"]),
                             mysql_escape_string($_POST["ip_nbios_ip_sent_bytes_persistence"])));
    $ip_nbios_ip_rcvd_bytes
        = implode(",", array(mysql_escape_string($_POST["ip_nbios_ip_rcvd_bytes_threshold"]),
                             mysql_escape_string($_POST["ip_nbios_ip_rcvd_bytes_priority"]),
                             mysql_escape_string($_POST["ip_nbios_ip_rcvd_bytes_alpha"]),
                             mysql_escape_string($_POST["ip_nbios_ip_rcvd_bytes_beta"]),
                             mysql_escape_string($_POST["ip_nbios_ip_rcvd_bytes_persistence"])));
    $ip_mail_sent_bytes
        = implode(",", array(mysql_escape_string($_POST["ip_mail_sent_bytes_threshold"]),
                             mysql_escape_string($_POST["ip_mail_sent_bytes_priority"]),
                             mysql_escape_string($_POST["ip_mail_sent_bytes_alpha"]),
                             mysql_escape_string($_POST["ip_mail_sent_bytes_beta"]),
                             mysql_escape_string($_POST["ip_mail_sent_bytes_persistence"])));
    $ip_mail_rcvd_bytes
        = implode(",", array(mysql_escape_string($_POST["ip_mail_rcvd_bytes_threshold"]),
                             mysql_escape_string($_POST["ip_mail_rcvd_bytes_priority"]),
                             mysql_escape_string($_POST["ip_mail_rcvd_bytes_alpha"]),
                             mysql_escape_string($_POST["ip_mail_rcvd_bytes_beta"]),
                             mysql_escape_string($_POST["ip_mail_rcvd_bytes_persistence"])));
    $mrtg_a
        = implode(",", array(mysql_escape_string($_POST["mrtg_a_threshold"]),
                             mysql_escape_string($_POST["mrtg_a_priority"]),
                             mysql_escape_string($_POST["mrtg_a_alpha"]),
                             mysql_escape_string($_POST["mrtg_a_beta"]),
                             mysql_escape_string($_POST["mrtg_a_persistence"])));
    $mrtg_c
        = implode(",", array(mysql_escape_string($_POST["mrtg_c_threshold"]),
                             mysql_escape_string($_POST["mrtg_c_priority"]),
                             mysql_escape_string($_POST["mrtg_c_alpha"]),
                             mysql_escape_string($_POST["mrtg_c_beta"]),
                             mysql_escape_string($_POST["mrtg_c_persistence"])));

    require_once 'ossim_db.inc';
    require_once 'classes/RRD_conf.inc';
    $db = new ossim_db();
    $conn = $db->connect();

    RRD_conf::update ($conn, $ip, $pkt_sent, $pkt_rcvd, 
                      $bytes_sent, $bytes_rcvd,
                      $tot_contacted_sent_peers, $tot_contacted_rcvd_peers, 
                      $ip_dns_sent_bytes, $ip_dns_rcvd_bytes, 
                      $ip_nbios_ip_sent_bytes, $ip_nbios_ip_rcvd_bytes,
                      $ip_mail_sent_bytes, $ip_mail_rcvd_bytes, 
                      $mrtg_a, $mrtg_c);
    }

    $db->close($conn);
}
?>
    <p>RRD Conf succesfully updated</p>
    <p><a href="rrd_conf.php">Back</a></p>

</body>
</html>

