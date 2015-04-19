<?php
require_once 'classes/Session.inc';
Session::logcheck("MenuControlPanel", "ControlPanelMetrics");

require_once 'classes/Util.inc';
require_once 'classes/Net.inc';
require_once 'classes/Security.inc';
require_once 'ossim_conf.inc';

$conf = $GLOBALS["CONF"];
$acid_link = $conf->get_conf("acid_link");

$type=GET('type');
$start=GET('start');
$end=GET('end');
$range = GET('range');

ossim_valid($type,  OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("type"));
ossim_valid($start, OSS_DIGIT, 'illegal:'._('Start param'));
ossim_valid($end,   OSS_DIGIT, 'illegal:'._('End param'));

$valid_range = array('day', 'week', 'month', 'year');

if (!$range) {
    $range = 'day';
} elseif (!in_array($range, $valid_range)) {
    die(ossim_error('Invalid range'));
}


if ($range == 'day') {
    $rrd_start = "N-1D";
} elseif ($range == 'week') {
    $rrd_start = "N-7D";
} elseif ($range == 'month') {
    $rrd_start = "N-1M";
} elseif ($range == 'year') {
    $rrd_start = "N-1Y";
}


$db = new ossim_db();
$conn = $db->connect();

// Get conf
$conf = $GLOBALS['CONF'];
$rrdtool_bin = $conf->get_conf('rrdtool_path') . "/rrdtool";

switch ($type) {
    case 'host':
        $rrdpath = $conf->get_conf('rrdpath_host'); break;
    case 'net':
        $rrdpath = $conf->get_conf('rrdpath_net'); break;
    case 'global':
        $rrdpath = $conf->get_conf('rrdpath_global'); break;
    case 'level':
        $rrdpath = $conf->get_conf('rrdpath_level'); break;
}

function RemoveExtension($strName, $strExt){

if(substr($strName,strlen($strName) - strlen($strExt)) == $strExt)
    return substr($strName,0,strlen($strName) - strlen($strExt));
else
    return $strName;

}

// Open dir and get files list
if (is_dir($rrdpath)) {
   if ($gestordir = opendir($rrdpath)) {
      $i = 0;
      $nrrds = 0;
      $rrds = array();
      while (($rrdfile = readdir($gestordir)) !== false) {

        if (strcmp($rrdfile,"..")==0 || strcmp($rrdfile,".")==0){
                continue;
        }

        $file_date=filemtime($rrdpath . $rrdfile);

        // Get files list modified after start date
        if (isset($start) && ($file_date > $start)){
                $i++;
                $command = "$rrdtool_bin fetch $rrdpath/$rrdfile MAX -s $start -e $end";
                $handle = popen($command,"r");
                if ($handle) {
                   while (!feof($handle)) {
                       $buffer = fgets($handle, 4096);
                        //9.9650777833e+01
                        if(preg_match("/(\d+):\s+(\d+\.\d+e\+\d+)\s+(\d+\.\d+e\+\d+)/", $buffer, $out)){
                                if ($out[2] > 0) {
//                                      echo "$rrdfile at " . date("Y-m-d H:i:s",$out[1]) . " -> C: " . intval(floatval($out[2])) . "<br>";
                                        array_push($rrds, $rrdfile);
                                        $nrrds++;
                                        break;
                                }
                                if ($out[3] > 0) {
//                                      echo "$rrdfile at " . date("Y-m-d H:i:s",$out[1]) . " -> A: " . intval(floatval($out[3])) . "<br>";
                                        array_push($rrds, $rrdfile);
                                        $nrrds++;
                                        break;
                                }
                        }
                   }
                   pclose($handle);
                }
        }
      }
//      echo "<br>$i files older than ". date("Y-m-d H:i:s",$start)."<br>" ;    
      for ($i=0; $i<$nrrds;$i++) {
        $ip=RemoveExtension($rrds[$i], ".rrd");
        $what="compromise";
        $start_acid = date("Y-m-d H:i:s",$start);
        $end_acid = date("Y-m-d H:i:s",$end);
        ?>
        <center>
        <a href="<?=Util::get_acid_events_link($start_acid,$end_acid,"time_d",$ip,"ip_both");?>">
<!--      <img src="<?php echo "../report/graphs/draw_rrd.php?ip=$ip&what=$what&start=$start&end=$end&type=$type"; ?>" border=0> -->
          <img src="<?php echo "../report/graphs/draw_rrd.php?ip=$ip&what=$what&start=$rrd_start&end=N&type=$type"; ?>" border=0>
        </a>
        </center>
        <?php
      }
      closedir($gestordir);
   }
}
?>

