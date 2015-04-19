<?php
require_once 'classes/Session.inc';
require_once 'classes/Security.inc';
Session::logcheck("MenuIncidents", "IncidentsIncidents");
require_once 'ossim_db.inc';
require_once 'classes/Incident.inc';
require_once 'classes/Incident_alarm.inc';
require_once 'classes/Incident_event.inc';
require_once 'classes/Incident_metric.inc';
require_once 'classes/Incident_anomaly.inc';
require_once 'classes/Incident_vulnerability.inc';

$db = new ossim_db();
$conn = $db->connect();

$edit = GET('action') && GET('action') == 'edit' ? true : false;

$ref  = !ossim_valid(GET('ref'), OSS_LETTER) ? die("Ref required") : GET('ref'); 

if ($edit) {
    if (!ossim_valid(GET('incident_id'), OSS_DIGIT)) {
        die("Wrong ID");
    }
    $incident_id = GET('incident_id');
    $list = Incident::get_list($conn, "WHERE incident.id=$incident_id");
    if (count($list) != 1) die("Wrong ID");
    $incident = $list[0];
    $title = $incident->get_title();
    $priority = $incident->get_priority();
    $event_start = $incident->get_event_start();
    $event_end = $incident->get_event_end();
    $type = $incident->get_type();
    switch ($ref) {
        case 'Alarm':
            list($alarm) = Incident_alarm::get_list($conn, "WHERE incident_alarm.incident_id=$incident_id");
            $src_ips = $alarm->get_src_ips();
            $dst_ips = $alarm->get_dst_ips();
            $src_ports = $alarm->get_src_ports();
            $dst_ports = $alarm->get_dst_ports();
            break;
        case 'Event':
            list($event) = Incident_event::get_list($conn, "WHERE incident_event.incident_id=$incident_id");
            $src_ips = $event->get_src_ips();
            $dst_ips = $event->get_dst_ips();
            $src_ports = $event->get_src_ports();
            $dst_ports = $event->get_dst_ports();
            break;
        case 'Metric':
            list($metric) = Incident_metric::get_list($conn, "WHERE incident_metric.incident_id=$incident_id");
            $target = $metric->get_target();
            $metric_type = $metric->get_metric_type();
            $metric_value = $metric->get_metric_value();
            break;
        case 'Anomaly':
            list($anomaly) = Incident_anomaly::get_list($conn, "WHERE incident_anomaly.incident_id=$incident_id");
            $anom_type = $anomaly->get_anom_type();
            $anom_ip = $anomaly->get_ip();
            $anom_data_orig = $anomaly->get_data_orig();
            $anom_data_new  = $anomaly->get_data_new();

            if ($anom_type == "mac") {
                list($a_sen, $a_date, $a_mac_o, $a_vend_o) = explode(",", $anom_data_orig);
                list($a_sen, $a_date, $a_mac, $a_vend) = explode(",", $anom_data_new);
            } elseif ($anom_type == "service"){
                list($a_sen, $a_date, $a_port, $a_prot_o, $a_ver_o) = explode(",", $anom_data_orig);
                list($a_sen, $a_date, $a_port, $a_prot, $a_ver) = explode(",", $anom_data_new);
            } elseif ($anom_type == "os"){
                list($a_sen, $a_date, $a_os_o) = explode(",", $anom_data_orig);
                list($a_sen, $a_date, $a_os) = explode(",", $anom_data_new);
            }
            break;
        case 'Vulnerability':
            list($vulnerability) = Incident_vulnerability::get_list($conn, "WHERE incident_vulns.incident_id=$incident_id");
            $ip = $vulnerability->get_ip();
            $port = $vulnerability->get_port();
            $nessus_id = $vulnerability->get_nessus_id();
            $risk = $vulnerability->get_risk();
            $description = $vulnerability->get_description();
            break;
    }
} else {
    $title = GET('title');
    $priority = GET('priority');
    $type = GET('type');
    $src_ips = GET('src_ips');
    $dst_ips = GET('dst_ips');
    $src_ports = GET('src_ports');
    $dst_ports = GET('dst_ports');
    $target = GET('target');
    $event_start = GET('event_start');
    $event_end = GET('event_end');
    $metric_type = GET('metric_type');
    $metric_value = GET('metric_value');
    $anom_type  = GET('anom_type');
    $anom_ip    = GET('anom_ip');
    $a_sen      = GET('a_sen');
    $a_date     = GET('a_date');
    $a_mac_o    = GET('a_mac_o');
    $a_mac      = GET('a_mac');
    $a_vend_o   = GET('a_vend_o');
    $a_vend     = GET('a_vend');
    $a_ver_o    = GET('a_ver_o');
    $a_ver      = GET('a_ver');
    $a_port     = GET('a_port');
    $a_prot_o   = GET('a_prot_o');
    $a_prot     = GET('a_prot');
    $a_os_o     = GET('a_os_o');
    $a_os       = GET('a_os');
    $ip         = GET('ip');
    $port       = GET('port');
    $nessus_id  = GET('nessus_id');
    $risk       = GET('risk');
    $description = GET('description');
}
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

<h1><?=" $ref ". _("Incident")?></h1>

<form method="GET" action="manageincident.php">
<input type="hidden" name="action" value="<?= ($edit) ? 'editincident' : 'newincident'?>" />
<input type="hidden" name="ref" value="<?=$ref?>" />
<input type="hidden" name="incident_id" value="<?=$incident_id?>" />
<table align="center">
  <tr>
    <th><?=_("Title")?></th>
    <td>
      <input type="text" name="title" size="40" value="<?=$title?>" />
    </td>
  </tr>
  <tr>
    <th><?=_("Priority")?></th>
    <td class="left">
      <select name="priority">
<?php
        $options = "";
        for ($i = 1; $i <= 10; $i++) {
            $options .= "<option value=\"$i\"";
            if ($priority == $i) {
                $options .= " selected ";
            }
            $options .= ">$i</option>";
        }
        print $options;
?>
      </select>
    </td>
  </tr>
  <tr>
    <th><?= _("Type") ?></th>
<?php
    Incident::print_td_incident_type($conn, $type);
?>
  </tr>

<?php
    if (($ref == "Alarm") or ($ref == "Event")) {
?>
  <tr>
    <th><?=_("Source Ips") ?></th>
    <td class="left">
      <input type="text" name="src_ips" value="<?=$src_ips?>" />
    </td>
  </tr>
  <tr>
    <th><?=_("Dest Ips") ?></th>
    <td class="left">
      <input type="text" name="dst_ips" value="<?=$dst_ips?>" />
    </td>
  </tr>
  <tr>
    <th><?=_("Source Ports") ?></th>
    <td class="left">
      <input type="text" name="src_ports" value="<?=$src_ports?>" />
    </td>
  </tr>
  <tr>
    <th><?=_("Dest Ports") ?></th>
    <td class="left">
      <input type="text" name="dst_ports" value="<?=$dst_ports?>" /></td>
  </tr>
  <tr>
    <th><?=_("Start of related events") ?></th>
    <td class="left">
      <input type="text" name="event_start" value="<?=$event_start?>" /></td>
  </tr>
  <tr>
    <th><?=_("End of related events") ?></th>
    <td class="left">
      <input type="text" name="event_end" value="<?=$event_end?>" /></td>
  </tr>

<?php
    } elseif ($ref == "Metric") {
?>
  <tr>
    <th><?=_("Target (net, ip, etc)") ?></th>
    <td class="left">
      <input type="text" name="target" value="<?=$target?>" />
    </td>
  </tr>
  <tr>
    <th><?=_("Metric type") ?></th>
    <td class="left">
      <select name="metric_type">
        <option value="Compromise"
        <?php if ($metric_type == "Compromise") echo " selected "; ?>
            >Compromise</option>
        <option value="Attack"
        <?php if ($metric_type == "Attack") echo " selected "; ?>
            >Attack</option>
        <option value="Level"
        <?php if ($metric_type == "Level") echo " selected "; ?>
            >Level</option>
      </select>
    </td>
  </tr>
  <tr>
    <th><?=_("Metric value") ?></th>
    <td class="left">
      <input type="text" name="metric_value" value="<?=$metric_value?>" />
    </td>
  </tr>
  <tr>
    <th><?=_("Start of related events") ?></th>
    <td class="left">
      <input type="text" name="event_start" value="<?=$event_start?>" /></td>
  </tr>
  <tr>
    <th><?=_("End of related events") ?></th>
    <td class="left">
      <input type="text" name="event_end" value="<?=$event_end?>" /></td>
  </tr>
<?php
    } elseif ($ref == "Anomaly") {
?>
  <tr>
    <th><?=_("Anomaly type") ?></th>
    <td class="left">
      <input type="text" name="anom_type" size="30" value="<?=$anom_type?>" />
    </td>
  </tr>
    <tr>
    <th><?=_("Host") ?></th>
    <td class="left">
      <input type="text" name="anom_ip" size="30" value="<?=$anom_ip?>" />
    </td>
  </tr>
 <tr>
    <th><?=_("Sensor") ?></th>
    <td class="left">
      <input type="text" name="a_sen" size="30" value="<?=$a_sen?>" />
    </td>
  </tr>
<?php
    if ($anom_type == "os" ) {
?>
   <tr>
    <th><?=_("Old OS") ?></th>
    <td class="left">
      <input type="text" name="a_os_o" size="30" value="<?=$a_os_o?>" />
    </td>
  </tr>
    <tr>
    <th><?=_("New OS") ?></th>
    <td class="left">
      <input type="text" name="a_os"  size="30" value="<?=$a_os?>" />
    </td>
  </tr>
   <tr>
    <th><?=_("When") ?></th>
    <td class="left">
      <input type="text" name="a_date" size="30" value="<?=$a_date?>" />
    </td>
  </tr>

     
<?php        
    } elseif ($anom_type == "mac") {
?>
   <tr>
    <th><?=_("Old mac") ?></th>
    <td class="left">
      <input type="text" name="a_mac_o" size="30" value="<?=$a_mac_o?>" />
    </td>
  </tr>
    <tr>
    <th><?=_("New mac") ?></th>
    <td class="left">
      <input type="text" name="a_mac" size="30" value="<?=$a_mac?>" />
    </td>
  </tr>
    <tr>
    <th><?=_("Old vendor") ?></th>
    <td class="left">
      <input type="text" name="a_vend_o" size="30" value="<?=$a_vend_o?>" />
    </td>
  </tr>
    <tr>
    <th><?=_("New vendor") ?></th>
    <td class="left">
      <input type="text" name="a_vend" size="30" value="<?=$a_vend?>" />
    </td>
  </tr>
  <tr>
    <th><?=_("When") ?></th>
    <td class="left">
      <input type="text" name="a_date" size="30" value="<?=$a_date?>" />
    </td>
  </tr>


<?php
    } elseif ($anom_type == "service") {
?>

  <tr>
    <th><?=_("Port") ?></th>
    <td class="left">
      <input type="text" name="a_port" value="<?=$a_port?>" />
    </td>
  </tr>
    <tr>
    <th><?=_("Old Protocol") ?></th>
    <td class="left">
      <input type="text" name="a_prot_o" size="30" value="<?=$a_prot_o?>" />
    </td>
  </tr>
     <tr>
    <th><?=_("Old Version") ?></th>
    <td class="left">
      <input type="text" name="a_ver_o" size="30" value="<?=$a_ver_o?>" />
    </td>
  </tr>
    <tr>
    <th><?=_("New Protocol") ?></th>
    <td class="left">
      <input type="text" name="a_prot" size="30" value="<?=$a_prot?>" />
    </td>
  </tr>
     <tr>
    <th><?=_("New Version") ?></th>
    <td class="left">
      <input type="text" name="a_ver" size="30" value="<?=$a_ver?>" />
    </td>
  </tr>
    <tr>
    <th><?=_("When") ?></th>
    <td class="left">
      <input type="text" name="a_date" size="30" value="<?=$a_date?>" />
    </td>
  </tr>

<?php 
    }
?>


<?php
    } elseif ($ref == "Vulnerability") {
?>

 <tr>
    <th><?=_("IP") ?></th>
    <td class="left">
      <input type="text" name="ip" value="<?=$ip?>" />
    </td>
  </tr>
    <tr>
    <th><?=_("Port") ?></th>
    <td class="left">
      <input type="text" name="port" size="30" value="<?=$port?>" />
    </td>
  </tr>
     <tr>
    <th><?=_("Nessus ID") ?></th>
    <td class="left">
      <input type="text" name="nessus_id" size="30" value="<?=$nessus_id?>" />
    </td>
  </tr>
    <tr>
    <th><?=_("Risk") ?></th>
    <td class="left">
      <input type="text" name="risk" size="30" value="<?=$risk?>" />
    </td>
  </tr>
     <tr>
    <th><?=_("Description") ?></th>
    <td style="border-width: 0px;">
        <textarea name="description" rows="10" cols="80" wrap="hard"><?=$description?></textarea>
    </td>
  </tr>

<?php
    }
?>

<tr>
    <td colspan="2">
      <input type="submit" value="OK" />
    </td>
  </tr>
</table>
</form>

</body>
</html>
