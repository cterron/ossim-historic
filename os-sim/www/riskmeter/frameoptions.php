<?php
    require_once ('ossim_conf.inc');
    $conf = new ossim_conf();
    $acid_link = $conf->get_conf("acid_link");
    $acid_prefix = $conf->get_conf("alert_viewer");
    $ip = $_GET["ip"];
?>
<html>
<frameset rows="15%,85%" border="0" frameborder="0">
<frame src="options.php?ip=<?php echo $ip?>">
<frame src="<?php echo "$acid_link/".$acid_prefix."_qry_main.php?new=2&num_result_rows=-1&submit=Query+DB&current_view=-1&ip_addr_cnt=2&ip_addr%5B0%5D%5B0%5D=+&ip_addr%5B0%5D%5B1%5D=ip_src&ip_addr%5B0%5D%5B2%5D=%3D&ip_addr%5B0%5D%5B3%5D=$ip&ip_addr%5B0%5D%5B8%5D=+&ip_addr%5B0%5D%5B9%5D=OR&ip_addr%5B1%5D%5B0%5D=+&ip_addr%5B1%5D%5B1%5D=ip_dst&ip_addr%5B1%5D%5B2%5D=%3D&ip_addr%5B1%5D%5B3%5D=$ip&ip_addr%5B1%5D%5B8%5D=+&ip_addr%5B1%5D%5B9%5D=+&sort_order=time_d"?>" 
       name="main">
<body>
</body>
</html>

