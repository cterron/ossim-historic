<?php
require_once ('classes/Session.inc');
require_once 'classes/Util.inc';
require_once 'ossim_db.inc';
require_once ('classes/Log_action.inc');

class Backup {

function Insert($insert){
    $conf = $GLOBALS["CONF"];
    $data_dir = $conf->get_conf("data_dir");
    $backup_dir = $conf->get_conf("backup_dir");
    
    $files = Backup::array2str ($insert);
    if ($files != "") {
          $user = Session::get_session_user();
          $files = escapeshellcmd($files);
          $user = escapeshellcmd($user);
          $res = popen("$data_dir/scripts/restoredb.pl insert $files $user &", "r");
          $isDisabled = 1;
    }
    $infolog = array($files);
    Log_action::log(44,$infolog);
}


function Delete($delete){
    $conf = $GLOBALS["CONF"];
    $data_dir = $conf->get_conf("data_dir");
    $backup_dir = $conf->get_conf("backup_dir");
        
    $files = Backup::array2str ($delete);
    if ($files != "") {
          $user = Session::get_session_user();
          $files = escapeshellcmd($files);
          $user = escapeshellcmd($user);
          $res = popen("$data_dir/scripts/restoredb.pl delete $files $user &", "r");
          $isDisabled = 1;
          sleep(3);
    }
    $infolog = array($files);
    Log_action::log(45,$infolog);
}

function str2date($str) {
	list($year, $month, $day) = sscanf($str, "%4s%2s%2s");
	return "$year-$month-$day";
}


function array2str ($arr) {
	$str = "";
    if(is_array($arr)){
	while (list($key, $value) = each ($arr)) {
		if ($str == "")
   		$str = $value;
   	else
   		$str .= "," . $value;
   }}
   return $str;
}

function running_restoredb(){
    if (file_exists("/tmp/ossim-restoredb.pid")) {
        return 1;
    } else {
        return 0;
    }
}



} //class backup

?>

