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

