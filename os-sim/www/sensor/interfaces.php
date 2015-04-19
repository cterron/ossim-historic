<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicySensors");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

<?php

$sensor = GET('sensor');
$interface = GET('interface');
$name = GET('name');
$main = GET('main');
$submit = GET('submit');


ossim_valid($sensor, OSS_ALPHA, OSS_SPACE, OSS_PUNC, 'illegal:'._("Sensor"));
ossim_valid($interface, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("Interface"));
ossim_valid($name, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_NULLABLE, 'illegal:'._("Sensor Name"));
ossim_valid($submit, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("Submit"));

if (ossim_error()) {
    die(ossim_error());
}


if(GET('submit')) {

    $temp_msg    = "inserted.";

    require_once 'ossim_db.inc';
    require_once 'classes/Sensor.inc';
    require_once 'classes/Sensor_interfaces.inc';
    $db = new ossim_db();
    $conn = $db->connect();
    if($submit == "Insert"){
    Sensor_interfaces::insert_interfaces($conn,$sensor,$interface,$name,$main);
    } elseif ($submit == "Update"){
    $temp_msg = gettext("updated") . " .";
    Sensor_interfaces::update_interfaces($conn,$sensor,$interface,$name,$main);
    } elseif ($submit == "Delete"){
    Sensor_interfaces::delete_interfaces($conn,$sensor,$interface);
    $temp_msg = gettext("deleted") . " .";
    }
    
//    Sensor::update ($conn, $name, $ip, $priority, $port, $descr);

    $db->close($conn);
?>
    <p> <?php echo gettext("Interface succesfully"); ?> <?php echo $temp_msg;?></p>
    <p><a href="sensor.php"> <?php echo gettext("Back"); ?> </a></p>


<?php
}
require_once ('ossim_conf.inc');
require_once ('ossim_db.inc');
require_once ('classes/Sensor.inc');
require_once ('classes/Sensor_interfaces.inc');
require_once ('classes/SecurityReport.inc');
$conf = $GLOBALS["CONF"];
$db = new ossim_db();
$conn = $db->connect();

$updating = 0;

if($sensor_interface_list = Sensor_interfaces::get_list($conn, $sensor)){
$updating = 1;
}

if($updating)
{
?>

  <h1> <?php echo gettext("Update interfaces for"); ?> <?php echo $sensor; ?></h1>

<table align="center">
<tr><th> <?php echo gettext("Interface"); ?> </th><th> 
<?php echo gettext("Name"); ?> </th><th> 
<?php echo gettext("Main"); ?> </th><th> 
<?php echo gettext("Action"); ?> </th></tr>
<?php
foreach($sensor_interface_list as $s_int){
?>
<form method="GET" action="interfaces.php">
<input type="hidden" name="sensor" value="<?php echo $sensor;?>">
<input type="hidden" name="interface" value="<?php echo $s_int->get_interface();?>">
<tr>
<td><?php echo $s_int->get_interface();?></td>
<td><input type="text" name="name" value="<?php echo $s_int->get_name(); ?>"></td>
<td><select name="main">
<?php
if($s_int->get_main()){
?>
<option value="1" selected> <?php echo gettext("Yes"); ?> </option>
<option value="0"> <?php echo gettext("No"); ?> </option>
<?php
}else{
?>
<option value="0" selected> <?php echo gettext("No"); ?> </option>
<option value="1"> <?php echo gettext("Yes"); ?> </option>
<?php
}
?>
</select></td>
<td><input type="submit" name="submit" value="Update"><input
type="submit" name="submit" value="Delete"></td>
</tr>
</form>
<?php
}
?>
<form method="GET" action="interfaces.php">
<input type="hidden" name="sensor" value="<?php echo $sensor;?>">
<tr>
<td><input type="text" name="interface"</td>
<td><input type="text" name="name"></td>
<td><select name="main">
<option value="0" selected> <?php echo gettext("No"); ?> </option>
<option value="1"> <?php echo gettext("Yes"); ?> </option>
</select></td>
<td><input type="submit" name="submit" value="Insert"></td>
</tr>
</table>
<?php
} else {
?>

  <h1>Insert interfaces for <?php echo $sensor; ?></h1>
<table align="center">
<form method="GET" action="interfaces.php">
<input type="hidden" name="sensor" value="<?php echo $sensor;?>">
<tr><th> <?php echo gettext("Interface"); ?> </th><th> 
<?php echo gettext("Name"); ?> </th><th> 
<?php echo gettext("Main"); ?> </th><th> 
<?php echo gettext("Action"); ?> </th></tr>
<tr>
<td><input type="text" name="interface"</td>
<td><input type="text" name="name"></td>
<td><select name="main">
<option value="1" selected> <?php echo gettext("Yes"); ?> </option>
<option value="0"> <?php echo gettext("No"); ?> </option>
</select></td>
<td><input type="submit" name="submit" value="Insert"></td>
</tr>
</table>

<?php
}

?>
    <p><a href="sensor.php"> <?php echo gettext("Back"); ?> </a></p>

</body>
</html>

