<?php
    require_once ('ossim_conf.inc');
    $conf = new ossim_conf();
    $acid_link = $conf->get_conf("acid_link");
    $ip = $_GET["ip"];
?>
<html>
<frameset rows="15%,85%" border="0" frameborder="0">
<frame src="options.php?ip=<?php echo $ip?>">
<frame src="<?php echo "$acid_link/acid_stat_ipaddr.php?ip=$ip&netmask=32"?>" 
       name="main">
<body>
</body>
</html>

