<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

<?php
if (!$sensor = $_GET["sensor"])
{
    echo "<p align=\"center\">Argument missing</p>";
    exit();
}

require_once ('ossim_conf.inc');
$conf = new ossim_conf();

# get ntop port from default ntop entry at
# /etc/ossim/framework/ossim.conf
# a better solution ??
list($proto, $ip, $port) = split(':', $conf->get_conf("ntop_link"));

require_once ('ossim_db.inc');
require_once ('classes/Sensor.inc');

$db = new ossim_db();
$conn = $db->connect();
?>

<table align="center"><tr><td>
<form method="GET" action="menu_session.php">
<input type="hidden" name="proto" value="<?php echo $proto ?>"/>
<input type="hidden" name="port" value="<?php echo $port ?>"/>
Sensor:&nbsp;
<select name="sensor" onChange="submit()">
<?php
if ($sensor_list = Sensor::get_list($conn)) {
    foreach ($sensor_list as $s) {
?>
  <option 
<?php 
    if ($sensor == $s->get_ip()) echo " SELECTED ";
?>
    value="<?php echo $s->get_ip() ?>"><?php 
        echo $s->get_name() ?></option>
<?php
    }
}
?>
</select>
<?php
require_once ('ossim_conf.inc');
$conf = new ossim_conf();
?>
<a href="<?php echo "$proto://$sensor:$port"?>/NetNetstat.html"
       target="ntop">Reload</a>
</td></tr>
</form>
</table>

</body>
</html>

