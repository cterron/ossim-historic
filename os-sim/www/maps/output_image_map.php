<?php
require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
require_once 'classes/User_config.inc';
Session::logcheck("MenuConfiguration", "ConfigurationMaps");

$db = new ossim_db();
$conn = $db->connect();

if (GET('tmp_image')) {
    $config = new User_config($conn);
    $login = Session::get_session_user();
    
    header("Content-Type: ".$config->get($login, 'maps_tmp_image_type'));
    header("Content-Lenght: ".strlen($config->get($login, 'maps_tmp_image')));
    echo $config->get($login, 'maps_tmp_image');
    exit;
}
if ($map_id = GET('map_id')) {
    $sql = "SELECT engine_data1, engine_data2 FROM map WHERE id = ?";
    $row = $conn->GetRow($sql, array($map_id));
    
    header("Content-Type: ".$row['engine_data2']);
    echo $row['engine_data1'];
    exit;
}
?>
