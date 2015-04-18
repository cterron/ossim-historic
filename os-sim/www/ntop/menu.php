<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

<?php

require_once ('ossim_conf.inc');
$conf = new ossim_conf();

?>

<a href="<?php echo $conf->get_conf("ntop_link")?>/trafficStats.html"
       target="ntop">Global</a></br>
<a href="<?php echo $conf->get_conf("ntop_link")?>/sortDataProtos.html"
       target="ntop">Protocols</a><br/><br/>

<b>Services</b><br/><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo $conf->get_conf("ntop_link")?>/sortDataIP.html"
   target="ntop">By host: Total</a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo $conf->get_conf("ntop_link")?>/sortDataSentIP.html"
   target="ntop">By host: Sent</a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo $conf->get_conf("ntop_link")?>/sortDataReceivedIP.html"
   target="ntop">By host: Recv</a><br/>
   
&nbsp;&nbsp;&nbsp;
<a href="<?php echo $conf->get_conf("ntop_link")?>/ipProtoDistrib.html"
  target="ntop">Service statistic</a><br/>

&nbsp;&nbsp;&nbsp;
<a href="<?php echo $conf->get_conf("ntop_link")?>/ipProtoUsage.html"
  target="ntop">By client-server</a><br/><br/>

<b>Throughput</b><br/><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo $conf->get_conf("ntop_link")?>/sortDataThpt.html?col=1"
   target="ntop">By host: Total</a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo $conf->get_conf("ntop_link")?>/sortDataSentThpt.html?col=1"
   target="ntop">By host: Sent</a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo $conf->get_conf("ntop_link")?>/sortDataReceivedThpt.html?col=1"
   target="ntop">By host: Recv</a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo $conf->get_conf("ntop_link")?>/thptStats.html?col=1"
   target="ntop">Total (Graph)</a><br/><br/>

<b>Matrix</b><br/><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo $conf->get_conf("ntop_link")?>/ipTrafficMatrix.html"
   target="ntop">Data Matrix</a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo $conf->get_conf("ntop_link")?>/dataHostTraffic.html"
   target="ntop">Time Matrix</a><br/><br/>
   
<b>Gateways, VLANs</b><br/><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo $conf->get_conf("ntop_link")?>/localRoutersList.html"
   target="ntop">Gateways</a><br/>
&nbsp;&nbsp;&nbsp;
<a href="<?php echo $conf->get_conf("ntop_link")?>/vlanList.html"
   target="ntop">VLANs</a><br/><br/>

<a href="<?php echo $conf->get_conf("ntop_link")?>/localHostsInfo.html"
target="ntop">OS and Users</a><br/>

<a href="<?php echo $conf->get_conf("ntop_link")?>/domainTrafficStats.html"
target="ntop">Domains</a><br/>


</body>
</html>

