<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>OSSIM Framework</h1>

<?php

    /* check params */
    if (($_POST["insert"]) &&
        (!$_POST["ip"])) 
    {
?>

  <p align="center">Please, complete all the fields</p>
  <?php exit();?>

<?php

/* check OK, insert into BD */
} elseif($_POST["insert"]) {

    $ip = $_POST["ip"];

    if($ip == "global") {

	$active_host_senders_num
 		= implode(",", array($_POST["active_host_senders_num_threshold"],
			 $_POST["active_host_senders_num_priority"],
			 $_POST["active_host_senders_num_alpha"],
			 $_POST["active_host_senders_num_beta"]));
	$arp_rarp_bytes
 		= implode(",", array($_POST["arp_rarp_bytes_threshold"],
			 $_POST["arp_rarp_bytes_priority"],
			 $_POST["arp_rarp_bytes_alpha"],
			 $_POST["arp_rarp_bytes_beta"]));
	$broadcast_pkts
 		= implode(",", array($_POST["broadcast_pkts_threshold"],
			 $_POST["broadcast_pkts_priority"],
			 $_POST["broadcast_pkts_alpha"],
			 $_POST["broadcast_pkts_beta"]));
	$ethernet_bytes
 		= implode(",", array($_POST["ethernet_bytes_threshold"],
			 $_POST["ethernet_bytes_priority"],
			 $_POST["ethernet_bytes_alpha"],
			 $_POST["ethernet_bytes_beta"]));
	$ethernet_pkts
 		= implode(",", array($_POST["ethernet_pkts_threshold"],
			 $_POST["ethernet_pkts_priority"],
			 $_POST["ethernet_pkts_alpha"],
			 $_POST["ethernet_pkts_beta"]));
	$icmp_bytes
 		= implode(",", array($_POST["icmp_bytes_threshold"],
			 $_POST["icmp_bytes_priority"],
			 $_POST["icmp_bytes_alpha"],
			 $_POST["icmp_bytes_beta"]));
	$igmp_bytes
 		= implode(",", array($_POST["igmp_bytes_threshold"],
			 $_POST["igmp_bytes_priority"],
			 $_POST["igmp_bytes_alpha"],
			 $_POST["igmp_bytes_beta"]));
	$ip_bytes
 		= implode(",", array($_POST["ip_bytes_threshold"],
			 $_POST["ip_bytes_priority"],
			 $_POST["ip_bytes_alpha"],
			 $_POST["ip_bytes_beta"]));
	$ip_dhcp_bootp_bytes
 		= implode(",", array($_POST["ip_dhcp_bootp_bytes_threshold"],
			 $_POST["ip_dhcp_bootp_bytes_priority"],
			 $_POST["ip_dhcp_bootp_bytes_alpha"],
			 $_POST["ip_dhcp_bootp_bytes_beta"]));
	$ip_dns_bytes
 		= implode(",", array($_POST["ip_dns_bytes_threshold"],
			 $_POST["ip_dns_bytes_priority"],
			 $_POST["ip_dns_bytes_alpha"],
			 $_POST["ip_dns_bytes_beta"]));
	$ip_edonkey_bytes
 		= implode(",", array($_POST["ip_edonkey_bytes_threshold"],
			 $_POST["ip_edonkey_bytes_priority"],
			 $_POST["ip_edonkey_bytes_alpha"],
			 $_POST["ip_edonkey_bytes_beta"]));
	$ip_ftp_bytes
 		= implode(",", array($_POST["ip_ftp_bytes_threshold"],
			 $_POST["ip_ftp_bytes_priority"],
			 $_POST["ip_ftp_bytes_alpha"],
			 $_POST["ip_ftp_bytes_beta"]));
	$ip_gnutella_bytes
 		= implode(",", array($_POST["ip_gnutella_bytes_threshold"],
			 $_POST["ip_gnutella_bytes_priority"],
			 $_POST["ip_gnutella_bytes_alpha"],
			 $_POST["ip_gnutella_bytes_beta"]));
	$ip_http_bytes
 		= implode(",", array($_POST["ip_http_bytes_threshold"],
			 $_POST["ip_http_bytes_priority"],
			 $_POST["ip_http_bytes_alpha"],
			 $_POST["ip_http_bytes_beta"]));
	$ip_kazaa_bytes
 		= implode(",", array($_POST["ip_kazaa_bytes_threshold"],
			 $_POST["ip_kazaa_bytes_priority"],
			 $_POST["ip_kazaa_bytes_alpha"],
			 $_POST["ip_kazaa_bytes_beta"]));
	$ip_mail_bytes
 		= implode(",", array($_POST["ip_mail_bytes_threshold"],
			 $_POST["ip_mail_bytes_priority"],
			 $_POST["ip_mail_bytes_alpha"],
			 $_POST["ip_mail_bytes_beta"]));
	$ip_messenger_bytes
 		= implode(",", array($_POST["ip_messenger_bytes_threshold"],
			 $_POST["ip_messenger_bytes_priority"],
			 $_POST["ip_messenger_bytes_alpha"],
			 $_POST["ip_messenger_bytes_beta"]));
	$ip_nbios_ip_bytes
 		= implode(",", array($_POST["ip_nbios_ip_bytes_threshold"],
			 $_POST["ip_nbios_ip_bytes_priority"],
			 $_POST["ip_nbios_ip_bytes_alpha"],
			 $_POST["ip_nbios_ip_bytes_beta"]));
	$ip_nfs_bytes
 		= implode(",", array($_POST["ip_nfs_bytes_threshold"],
			 $_POST["ip_nfs_bytes_priority"],
			 $_POST["ip_nfs_bytes_alpha"],
			 $_POST["ip_nfs_bytes_beta"]));
	$ip_nttp_bytes
 		= implode(",", array($_POST["ip_nttp_bytes_threshold"],
			 $_POST["ip_nttp_bytes_priority"],
			 $_POST["ip_nttp_bytes_alpha"],
			 $_POST["ip_nttp_bytes_beta"]));
	$ip_snmp_bytes
 		= implode(",", array($_POST["ip_snmp_bytes_threshold"],
			 $_POST["ip_snmp_bytes_priority"],
			 $_POST["ip_snmp_bytes_alpha"],
			 $_POST["ip_snmp_bytes_beta"]));
	$ip_ssh_bytes
 		= implode(",", array($_POST["ip_ssh_bytes_threshold"],
			 $_POST["ip_ssh_bytes_priority"],
			 $_POST["ip_ssh_bytes_alpha"],
			 $_POST["ip_ssh_bytes_beta"]));
	$ip_telnet_bytes
 		= implode(",", array($_POST["ip_telnet_bytes_threshold"],
			 $_POST["ip_telnet_bytes_priority"],
			 $_POST["ip_telnet_bytes_alpha"],
			 $_POST["ip_telnet_bytes_beta"]));
	$ip_winmx_bytes
 		= implode(",", array($_POST["ip_winmx_bytes_threshold"],
			 $_POST["ip_winmx_bytes_priority"],
			 $_POST["ip_winmx_bytes_alpha"],
			 $_POST["ip_winmx_bytes_beta"]));
	$ip_x11_bytes
 		= implode(",", array($_POST["ip_x11_bytes_threshold"],
			 $_POST["ip_x11_bytes_priority"],
			 $_POST["ip_x11_bytes_alpha"],
			 $_POST["ip_x11_bytes_beta"]));
	$ipx_bytes
 		= implode(",", array($_POST["ipx_bytes_threshold"],
			 $_POST["ipx_bytes_priority"],
			 $_POST["ipx_bytes_alpha"],
			 $_POST["ipx_bytes_beta"]));
	$known_hosts_num
 		= implode(",", array($_POST["known_hosts_num_threshold"],
			 $_POST["known_hosts_num_priority"],
			 $_POST["known_hosts_num_alpha"],
			 $_POST["known_hosts_num_beta"]));
	$multicast_pkts
 		= implode(",", array($_POST["multicast_pkts_threshold"],
			 $_POST["multicast_pkts_priority"],
			 $_POST["multicast_pkts_alpha"],
			 $_POST["multicast_pkts_beta"]));
	$ospf_bytes
 		= implode(",", array($_POST["ospf_bytes_threshold"],
			 $_POST["ospf_bytes_priority"],
			 $_POST["ospf_bytes_alpha"],
			 $_POST["ospf_bytes_beta"]));
	$other_bytes
 		= implode(",", array($_POST["other_bytes_threshold"],
			 $_POST["other_bytes_priority"],
			 $_POST["other_bytes_alpha"],
			 $_POST["other_bytes_beta"]));
	$tcp_bytes
 		= implode(",", array($_POST["tcp_bytes_threshold"],
			 $_POST["tcp_bytes_priority"],
			 $_POST["tcp_bytes_alpha"],
			 $_POST["tcp_bytes_beta"]));
	$udp_bytes
 		= implode(",", array($_POST["udp_bytes_threshold"],
			 $_POST["udp_bytes_priority"],
			 $_POST["udp_bytes_alpha"],
			 $_POST["udp_bytes_beta"]));
	$up_to_1024_pkts
 		= implode(",", array($_POST["up_to_1024_pkts_threshold"],
			 $_POST["up_to_1024_pkts_priority"],
			 $_POST["up_to_1024_pkts_alpha"],
			 $_POST["up_to_1024_pkts_beta"]));
	$up_to_128_pkts
 		= implode(",", array($_POST["up_to_128_pkts_threshold"],
			 $_POST["up_to_128_pkts_priority"],
			 $_POST["up_to_128_pkts_alpha"],
			 $_POST["up_to_128_pkts_beta"]));
	$up_to_1518_pkts
 		= implode(",", array($_POST["up_to_1518_pkts_threshold"],
			 $_POST["up_to_1518_pkts_priority"],
			 $_POST["up_to_1518_pkts_alpha"],
			 $_POST["up_to_1518_pkts_beta"]));
	$up_to_512_pkts
 		= implode(",", array($_POST["up_to_512_pkts_threshold"],
			 $_POST["up_to_512_pkts_priority"],
			 $_POST["up_to_512_pkts_alpha"],
			 $_POST["up_to_512_pkts_beta"]));
	$up_to_64_pkts
 		= implode(",", array($_POST["up_to_64_pkts_threshold"],
			 $_POST["up_to_64_pkts_priority"],
			 $_POST["up_to_64_pkts_alpha"],
			 $_POST["up_to_64_pkts_beta"]));

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
        = implode(",", array($_POST["pkt_sent_threshold"],
                             $_POST["pkt_sent_priority"],
                             $_POST["pkt_sent_alpha"],
                             $_POST["pkt_sent_beta"]));
    $pkt_rcvd
        = implode(",", array($_POST["pkt_rcvd_threshold"],
                             $_POST["pkt_rcvd_priority"],
                             $_POST["pkt_rcvd_alpha"],
                             $_POST["pkt_rcvd_beta"]));
    $bytes_sent
        = implode(",", array($_POST["bytes_sent_threshold"],
                             $_POST["bytes_sent_priority"],
                             $_POST["bytes_sent_alpha"],
                             $_POST["bytes_sent_beta"]));
    $bytes_rcvd
        = implode(",", array($_POST["bytes_rcvd_threshold"],
                             $_POST["bytes_rcvd_priority"],
                             $_POST["bytes_rcvd_alpha"],
                             $_POST["bytes_rcvd_beta"]));
    $tot_contacted_sent_peers
        = implode(",", array($_POST["tot_contacted_sent_peers_threshold"],
                             $_POST["tot_contacted_sent_peers_priority"],
                             $_POST["tot_contacted_sent_peers_alpha"],
                             $_POST["tot_contacted_sent_peers_beta"]));
    $tot_contacted_rcvd_peers
        = implode(",", array($_POST["tot_contacted_rcvd_peers_threshold"],
                             $_POST["tot_contacted_rcvd_peers_priority"],
                             $_POST["tot_contacted_rcvd_peers_alpha"],
                             $_POST["tot_contacted_rcvd_peers_beta"]));
    $ip_dns_sent_bytes
        = implode(",", array($_POST["ip_dns_sent_bytes_threshold"],
                             $_POST["ip_dns_sent_bytes_priority"],
                             $_POST["ip_dns_sent_bytes_alpha"],
                             $_POST["ip_dns_sent_bytes_beta"]));
    $ip_dns_rcvd_bytes
        = implode(",", array($_POST["ip_dns_rcvd_bytes_threshold"],
                             $_POST["ip_dns_rcvd_bytes_priority"],
                             $_POST["ip_dns_rcvd_bytes_alpha"],
                             $_POST["ip_dns_rcvd_bytes_beta"]));
    $ip_nbios_ip_sent_bytes
        = implode(",", array($_POST["ip_nbios_ip_sent_bytes_threshold"],
                             $_POST["ip_nbios_ip_sent_bytes_priority"],
                             $_POST["ip_nbios_ip_sent_bytes_alpha"],
                             $_POST["ip_nbios_ip_sent_bytes_beta"]));
    $ip_nbios_ip_rcvd_bytes
        = implode(",", array($_POST["ip_nbios_ip_rcvd_bytes_threshold"],
                             $_POST["ip_nbios_ip_rcvd_bytes_priority"],
                             $_POST["ip_nbios_ip_rcvd_bytes_alpha"],
                             $_POST["ip_nbios_ip_rcvd_bytes_beta"]));
    $ip_mail_sent_bytes
        = implode(",", array($_POST["ip_mail_sent_bytes_threshold"],
                             $_POST["ip_mail_sent_bytes_priority"],
                             $_POST["ip_mail_sent_bytes_alpha"],
                             $_POST["ip_mail_sent_bytes_beta"]));
    $ip_mail_rcvd_bytes
        = implode(",", array($_POST["ip_mail_rcvd_bytes_threshold"],
                             $_POST["ip_mail_rcvd_bytes_priority"],
                             $_POST["ip_mail_rcvd_bytes_alpha"],
                             $_POST["ip_mail_rcvd_bytes_beta"]));
    $mrtg_a
        = implode(",", array($_POST["mrtg_a_threshold"],
                             $_POST["mrtg_a_priority"],
                             $_POST["mrtg_a_alpha"],
                             $_POST["mrtg_a_beta"]));
    $mrtg_c
        = implode(",", array($_POST["mrtg_c_threshold"],
                             $_POST["mrtg_c_priority"],
                             $_POST["mrtg_c_alpha"],
                             $_POST["mrtg_c_beta"]));

    require_once 'ossim_db.inc';
    require_once 'classes/RRD_conf.inc';
    $db = new ossim_db();
    $conn = $db->connect();

    RRD_conf::update ($conn, $ip, $pkt_sent, $pkt_rcvd, 
                      $bytes_sent, $bytes_rcvd,
                      $tot_contacted_sent_peers, $tot_contacted_rcvd_peers, 
                      $ip_dns_sent_bytes, $ip_dns_rcvd_bytes, 
                      $ip_nbios_ip_sent_bytes, $ip_nbios_ip_rcvd_bytes,
                      $ip_mail_rcvd_bytes, $ip_mail_rcvd_bytes, 
                      $mrtg_a, $mrtg_c);
    }

    $db->close($conn);
}
?>
    <p>RRD Conf succesfully updated</p>
    <p><a href="rrd_conf.php">Back</a></p>

</body>
</html>

