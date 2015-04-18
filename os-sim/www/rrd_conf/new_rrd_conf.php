<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>New RRD Config</h1>

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

    RRD_conf::insert ($conn, $ip, $pkt_sent, $pkt_rcvd, 
                      $bytes_sent, $bytes_rcvd,
                      $tot_contacted_sent_peers, $tot_contacted_rcvd_peers, 
                      $ip_dns_sent_bytes, $ip_dns_rcvd_bytes, 
                      $ip_nbios_ip_sent_bytes, $ip_nbios_ip_rcvd_bytes,
                      $ip_mail_rcvd_bytes, $ip_mail_rcvd_bytes, 
                      $mrtg_a, $mrtg_c);

    $db->close($conn);
}
?>
    <p>RRD Conf succesfully inserted</p>
    <p><a href="rrd_conf.php">Back</a></p>

</body>
</html>

