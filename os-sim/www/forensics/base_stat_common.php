<?php
/**
* Class and Function List:
* Function list:
* - SensorCnt()
* - SensorTotal()
* - EventCnt()
* - UniqueCntBySensor()
* - EventCntBySensor()
* - MinDateBySensor()
* - MaxDateBySensor()
* - UniqueDestAddrCntBySensor()
* - UniqueSrcAddrCntBySensor()
* - TCPPktCnt()
* - UDPPktCnt()
* - ICMPPktCnt()
* - PortscanPktCnt()
* - UniqueSrcIPCnt()
* - UniqueDstIPCnt()
* - UniqueIPCnt()
* - StartStopTime()
* - UniqueAlertCnt()
* - UniquePortCnt()
* - UniqueTCPPortCnt()
* - UniqueUDPPortCnt()
* - UniqueLinkCnt()
* - PrintGeneralStats()
* - plot_graphic()
* - range_graphic()
* Classes list:
*/
/*******************************************************************************
** OSSIM Forensics Console
** Copyright (C) 2009 OSSIM/AlienVault
** Copyright (C) 2004 BASE Project Team
** Copyright (C) 2000 Carnegie Mellon University
**
** (see the file 'base_main.php' for license details)
**
** Built upon work by Roman Danyliw <rdd@cert.org>, <roman@danyliw.com>
** Built upon work by the BASE Project Team <kjohnson@secureideas.net>
*/
defined('_BASE_INC') or die('Accessing this file directly is not allowed.');
include_once ("$BASE_path/includes/base_constants.inc.php");
function SensorCnt($db, $join = "", $where = "") {
    if ($join == "" && $where == "") $result = $db->baseExecute("SELECT sensors FROM event_stats ORDER BY timestamp DESC LIMIT 1");
    else $result = $db->baseExecute("SELECT COUNT(DISTINCT acid_event.sid) FROM acid_event $join $where");
    $myrow = $result->baseFetchRow();
    $num = $myrow[0];
    $result->baseFreeRows();
    return $num;
}
function SensorTotal($db) {
    $result = $db->baseExecute("SELECT sensors_total FROM event_stats ORDER BY timestamp DESC LIMIT 1");
    $myrow = $result->baseFetchRow();
    $num = $myrow[0];
    $result->baseFreeRows();
    return $num;
}
function EventCnt($db, $join = "", $where = "", $force_query = "") {
    if ($force_query != "") {
        $result = $db->baseExecute($force_query);
    } else {
        if ($join == "" && $where == "") $result = $db->baseExecute("SELECT total_events FROM event_stats ORDER BY timestamp DESC LIMIT 1");
        else $result = $db->baseExecute("SELECT COUNT(acid_event.sid) FROM acid_event $join $where");
    }
    $myrow = $result->baseFetchRow();
    $num = $myrow[0];
    $result->baseFreeRows();
    return $num;
}
/*
* Takes: Numeric sensor ID from the Sensor table (SID), and
*	  database connection.
*
* Returns: The number of unique alert descriptions for the
* 	    given sensor ID.
*
*/
function UniqueCntBySensor($sensorID, $db) {
    /* Calculate the Unique Alerts */
    $query = "SELECT COUNT(DISTINCT signature) FROM acid_event WHERE sid = '" . $sensorID . "'";
    $result = $db->baseExecute($query);
    if ($result) {
        $row = $result->baseFetchRow();
        $num = $row[0];
        $result->baseFreeRows();
    } else $num = 0;
    return $num;
}
/*
* Takes: Numeric sensor ID from the Sensor table (SID), and
*        database connection.
*
* Returns: The total number of alerts for the given sensor ID
*/
function EventCntBySensor($sensorID, $db) {
    $query = "SELECT count(*) FROM acid_event where sid = '" . $sensorID . "'";
    $result = $db->baseExecute($query);
    $myrow = $result->baseFetchRow();
    $num = $myrow[0];
    $result->baseFreeRows();
    return $num;
}
function MinDateBySensor($sensorID, $db) {
    $query = "SELECT min(timestamp) FROM acid_event WHERE sid= '" . $sensorID . "'";
    $result = $db->baseExecute($query);
    $myrow = $result->baseFetchRow();
    $num = $myrow[0];
    $result->baseFreeRows();
    return $num;
}
function MaxDateBySensor($sensorID, $db) {
    $query = "SELECT max(timestamp) FROM acid_event WHERE sid='" . $sensorID . "'";
    $result = $db->baseExecute($query);
    $myrow = $result->baseFetchRow();
    $num = $myrow[0];
    $result->baseFreeRows();
    return $num;
}
function UniqueDestAddrCntBySensor($sensorID, $db) {
    $query = "SELECT COUNT(DISTINCT ip_dst) from acid_event WHERE sid='" . $sensorID . "'";
    $result = $db->baseExecute($query);
    $row = $result->baseFetchRow();
    $num = $row[0];
    $result->baseFreeRows();
    return $num;
}
function UniqueSrcAddrCntBySensor($sensorID, $db) {
    $query = "SELECT COUNT(DISTINCT ip_src) from acid_event WHERE sid='" . $sensorID . "'";
    $result = $db->baseExecute($query);
    $row = $result->baseFetchRow();
    $num = $row[0];
    $result->baseFreeRows();
    return $num;
}
function TCPPktCnt($db) {
    $result = $db->baseExecute("SELECT tcp_events FROM event_stats ORDER BY timestamp DESC LIMIT 1");
    $myrow = $result->baseFetchRow();
    $num = $myrow[0];
    $result->baseFreeRows();
    return $num;
}
function UDPPktCnt($db) {
    $result = $db->baseExecute("SELECT udp_events FROM event_stats ORDER BY timestamp DESC LIMIT 1");
    $myrow = $result->baseFetchRow();
    $num = $myrow[0];
    $result->baseFreeRows();
    return $num;
}
function ICMPPktCnt($db) {
    $result = $db->baseExecute("SELECT icmp_events FROM event_stats ORDER BY timestamp DESC LIMIT 1");
    $myrow = $result->baseFetchRow();
    $num = $myrow[0];
    $result->baseFreeRows();
    return $num;
}
function PortscanPktCnt($db) {
    $result = $db->baseExecute("SELECT portscan_events FROM event_stats ORDER BY timestamp DESC LIMIT 1");
    $myrow = $result->baseFetchRow();
    $num = $myrow[0];
    $result->baseFreeRows();
    return $num;
}
function UniqueSrcIPCnt($db, $join = "", $where = "") {
    if ($join == "" && $where == "") $result = $db->baseExecute("SELECT src_ips FROM event_stats ORDER BY timestamp DESC LIMIT 1");
    else $result = $db->baseExecute("SELECT COUNT(DISTINCT acid_event.ip_src) FROM acid_event $join WHERE $where"); //.
    //"WHERE acid_event.sid > 0 $where");
    $row = $result->baseFetchRow();
    $num = $row[0];
    $result->baseFreeRows();
    return $num;
}
function UniqueDstIPCnt($db, $join = "", $where = "") {
    if ($join == "" && $where == "") $result = $db->baseExecute("SELECT dst_ips FROM event_stats ORDER BY timestamp DESC LIMIT 1");
    else $result = $db->baseExecute("SELECT COUNT(DISTINCT acid_event.ip_dst) FROM acid_event $join WHERE $where"); //.
    //"WHERE acid_event.sid > 0 $where");
    $row = $result->baseFetchRow();
    $num = $row[0];
    $result->baseFreeRows();
    return $num;
}
function UniqueIPCnt($db, $join = "", $where = "") {
    $result = $db->baseExecute("SELECT src_ips, dst_ips FROM event_stats ORDER BY timestamp DESC LIMIT 1");
    $row = $result->baseFetchRow();
    $num1 = $row[0];
    $num2 = $row[1];
    $result->baseFreeRows();
    return array(
        $num1,
        $num2
    );
}
function StartStopTime(&$start_time, &$stop_time, $db) {
    /* mstone 20050309 special case postgres */
    if ($db->DB_type != "postgres") {
        $result = $db->baseExecute("SELECT min(timestamp), max(timestamp) FROM acid_event");
    } else {
        $result = $db->baseExecute("SELECT (SELECT timestamp FROM acid_event ORDER BY timestamp ASC LIMIT 1), (SELECT timestamp FROM acid_event ORDER BY timestamp DESC LIMIT 1)");
    }
    $myrow = $result->baseFetchRow();
    $start_time = $myrow[0];
    $stop_time = $myrow[1];
    $result->baseFreeRows();
}
function UniqueAlertCnt($db, $join = "", $where = "") {
    if ($join == "" && $where == "") {
        $result = $db->baseExecute("SELECT uniq_events FROM event_stats ORDER BY timestamp DESC LIMIT 1");
    } else {
        $result = $db->baseExecute("SELECT COUNT(DISTINCT acid_event.signature) FROM acid_event $join " . "$where");
    }
    $row = $result->baseFetchRow();
    $num = $row[0];
    $result->baseFreeRows();
    return $num;
}
function UniquePortCnt($db, $join = "", $where = "") {
    if ($join == "" && $where == "") $result = $db->baseExecute("SELECT source_ports, dest_ports FROM event_stats ORDER BY timestamp DESC LIMIT 1");
    else $result = $db->baseExecute("SELECT COUNT(DISTINCT acid_event.layer4_sport),  " . "COUNT(DISTINCT acid_event.layer4_dport) FROM acid_event $join " . "$where");
    $row = $result->baseFetchRow();
    $result->baseFreeRows();
    return array(
        $row[0],
        $row[1]
    );
}
function UniqueTCPPortCnt($db, $join = "", $where = "") {
    if ($join == "" && $where == "") $result = $db->baseExecute("SELECT source_ports_tcp, dest_ports_tcp  FROM event_stats ORDER BY timestamp DESC LIMIT 1");
    else $result = $db->baseExecute("SELECT COUNT(DISTINCT acid_event.layer4_sport),  " . "COUNT(DISTINCT acid_event.layer4_dport) FROM acid_event $join" . " $where AND ip_proto='" . TCP . "'");
    $row = $result->baseFetchRow();
    $result->baseFreeRows();
    return array(
        $row[0],
        $row[1]
    );
}
function UniqueUDPPortCnt($db, $join = "", $where = "") {
    if ($join == "" && $where == "") $result = $db->baseExecute("SELECT source_ports_udp, dest_ports_udp FROM event_stats ORDER BY timestamp DESC LIMIT 1");
    else $result = $db->baseExecute("SELECT COUNT(DISTINCT acid_event.layer4_sport),  " . "COUNT(DISTINCT acid_event.layer4_dport) FROM acid_event $join" . " $where AND ip_proto='" . UDP . "'");
    $row = $result->baseFetchRow();
    $result->baseFreeRows();
    return array(
        $row[0],
        $row[1]
    );
}
function UniqueLinkCnt($db, $join = "", $where = "") {
    if (!stristr($where, "WHERE") && $where != "") $where = " WHERE $where ";
    if ($db->DB_type == "mysql") {
        if ($join == "" && $where == "") $result = $db->baseExecute("SELECT uniq_ip_links  FROM event_stats ORDER BY timestamp DESC LIMIT 1");
        else $result = $db->baseExecute("SELECT COUNT(DISTINCT acid_event.ip_src, acid_event.ip_dst, acid_event.ip_proto) FROM acid_event $join $where");
        $row = $result->baseFetchRow();
        $result->baseFreeRows();
    } else {
        if ($join == "" && $where == "") $result = $db->baseExecute("SELECT DISTINCT acid_event.ip_src, acid_event.ip_dst, acid_event.ip_proto FROM acid_event");
        else $result = $db->baseExecute("SELECT DISTINCT acid_event.ip_src, acid_event.ip_dst, acid_event.ip_proto FROM acid_event $join $where");
        $row[0] = $result->baseRecordCount();
        $result->baseFreeRows();
    }
    return $row[0];
}
function PrintGeneralStats($db, $compact, $show_stats, $join = "", $where = "", $show_total_events = false) {
    if ($show_stats == 1) {
        $sensor_cnt = SensorCnt($db, $join, $where);
        $sensor_total = SensorTotal($db);
        $unique_alert_cnt = UniqueAlertCnt($db, $join, $where);
        $event_cnt = EventCnt($db, $join, $where);
        $unique_ip_cnt = UniqueIPCnt($db, $join, $where);
        $unique_links_cnt = UniqueLinkCnt($db, $join, $where);
        $unique_port_cnt = UniquePortCnt($db, $join, $where);
        $unique_tcp_port_cnt = UniqueTCPPortCnt($db, $join, $where);
        $unique_udp_port_cnt = UniqueUDPPortCnt($db, $join, $where);
    }
    if ($db->baseGetDBversion() >= 103) {
        /* mstone 20050309 this is an expensive calculation -- let's only do it if we're going to use it */
        if ($show_stats == 1) {
            $result = $db->baseExecute("SELECT categories FROM event_stats ORDER BY timestamp DESC LIMIT 1");
            $myrow = $result->baseFetchRow();
            $class_cnt = $myrow[0];
            $result->baseFreeRows();
        }
        $class_cnt_info[0] = " <strong>" . _SCCATEGORIES . " </strong>";
        $class_cnt_info[1] = "<a href=\"base_stat_class.php?sort_order=class_a\">";
        $class_cnt_info[2] = "</a><a href=\"base_stat_class_graph.php?sort_order=class_a\"> <img src=\"images/ico_graph.gif\" align=\"absmiddle\" border=0></a>";
    }
    $sensor_cnt_info[0] = "<strong>" . _SCSENSORTOTAL . "</strong>\n";
    $sensor_cnt_info[1] = "<a href=\"base_stat_sensor.php\">";
    $sensor_cnt_info[2] = "</a>";
    $unique_alert_cnt_info[0] = "<strong>" . _UNIALERTS . ":</strong>\n";
    $unique_alert_cnt_info[1] = "<a href=\"base_stat_alerts.php\">";
    $unique_alert_cnt_info[2] = "</a>";
    $event_cnt_info[0] = "<strong>" . _SCTOTALNUMALERTS . "</strong>\n";
    $event_cnt_info[1] = '<a href="base_qry_main.php?&amp;num_result_rows=-1' . '&amp;submit=' . _QUERYDBP . '&amp;current_view=-1">';
    $event_cnt_info[2] = "</a>";
    $unique_src_ip_cnt_info[0] = _SCSRCIP;
    $unique_src_ip_cnt_info[1] = " " . BuildUniqueAddressLink(1);
    $unique_src_ip_cnt_info[2] = "</a>";
    $unique_dst_ip_cnt_info[0] = _SCDSTIP;
    $unique_dst_ip_cnt_info[1] = " " . BuildUniqueAddressLink(2);
    $unique_dst_ip_cnt_info[2] = "</a>";
    $unique_links_info[0] = _SCUNILINKS;
    $unique_links_info[1] = " <a href=\"base_stat_iplink.php\">";
    $unique_links_info[2] = "</a>";
    $unique_src_port_cnt_info[0] = _SCSRCPORTS;
    $unique_src_port_cnt_info[1] = " <a href=\"base_stat_ports.php?port_type=1&amp;proto=-1\">";
    $unique_src_port_cnt_info[2] = "</a>";
    $unique_dst_port_cnt_info[0] = _SCDSTPORTS;
    $unique_dst_port_cnt_info[1] = " <a href=\"base_stat_ports.php?port_type=2&amp;proto=-1\">";
    $unique_dst_port_cnt_info[2] = "</a>";
    $unique_tcp_src_port_cnt_info[0] = "TCP (";
    $unique_tcp_src_port_cnt_info[1] = " <a href=\"base_stat_ports.php?port_type=1&amp;proto=" . TCP . "\">";
    $unique_tcp_src_port_cnt_info[2] = "</a>)";
    $unique_tcp_dst_port_cnt_info[0] = "TCP (";
    $unique_tcp_dst_port_cnt_info[1] = " <a href=\"base_stat_ports.php?port_type=2&amp;proto=" . TCP . "\">";
    $unique_tcp_dst_port_cnt_info[2] = "</a>)";
    $unique_udp_src_port_cnt_info[0] = "UDP (";
    $unique_udp_src_port_cnt_info[1] = " <a href=\"base_stat_ports.php?port_type=1&amp;proto=" . UDP . "\">";
    $unique_udp_src_port_cnt_info[2] = "</a>)";
    $unique_udp_dst_port_cnt_info[0] = "UDP (";
    $unique_udp_dst_port_cnt_info[1] = " <a href=\"base_stat_ports.php?port_type=2&amp;proto=" . UDP . "\">";
    $unique_udp_dst_port_cnt_info[2] = "</a>)";
    if ($show_stats == 1) {
        echo $sensor_cnt_info[0] . $sensor_cnt_info[1] . $sensor_cnt . $sensor_cnt_info[2] . $sensor_total . "\n<br />";
        echo $unique_alert_cnt_info[0] . $unique_alert_cnt_info[1] . $unique_alert_cnt . $unique_alert_cnt_info[2];
        if ($db->baseGetDBversion() >= 103) echo "<br />" . $class_cnt_info[0] . $class_cnt_info[1] . $class_cnt . $class_cnt_info[2];
        echo "<br />";
        echo $event_cnt_info[0] . $event_cnt_info[1] . $event_cnt . $event_cnt_info[2];
        echo "<ul>";
        echo "<li>" . $unique_src_ip_cnt_info[0] . $unique_src_ip_cnt_info[1] . $unique_ip_cnt[0] . $unique_src_ip_cnt_info[2] . "</li>";
        echo "<li>" . $unique_dst_ip_cnt_info[0] . $unique_dst_ip_cnt_info[1] . $unique_ip_cnt[1] . $unique_dst_ip_cnt_info[2] . "</li>";
        echo "<li>" . $unique_links_info[0] . $unique_links_info[1] . $unique_links_cnt . $unique_links_info[2] . "</li>";
        echo "<li>";
        if ($compact == 0) echo "<p>";
        echo $unique_src_port_cnt_info[0] . $unique_src_port_cnt_info[1] . $unique_port_cnt[0] . $unique_src_port_cnt_info[2] . "</li>";
        if ($compact == 0) echo "<li><ul><li>";
        else echo "<li>&nbsp;&nbsp;--&nbsp;&nbsp;";
        echo $unique_tcp_src_port_cnt_info[0] . $unique_tcp_src_port_cnt_info[1] . $unique_tcp_port_cnt[0] . $unique_tcp_src_port_cnt_info[2] . "&nbsp;&nbsp;" . $unique_udp_src_port_cnt_info[0] . $unique_udp_src_port_cnt_info[1] . $unique_udp_port_cnt[0] . $unique_udp_src_port_cnt_info[2];
        if ($compact == 0) echo "</li></ul></li>";
        echo "<li>" . $unique_dst_port_cnt_info[0] . $unique_dst_port_cnt_info[1] . $unique_port_cnt[1] . $unique_dst_port_cnt_info[2] . "</li>";
        if ($compact == 0) echo "<li><ul><li>";
        else echo "<li>&nbsp;&nbsp;--&nbsp;&nbsp;";
        echo $unique_tcp_dst_port_cnt_info[0] . $unique_tcp_dst_port_cnt_info[1] . $unique_tcp_port_cnt[1] . $unique_tcp_dst_port_cnt_info[2] . "&nbsp;&nbsp;" . $unique_udp_dst_port_cnt_info[0] . $unique_udp_dst_port_cnt_info[1] . $unique_udp_port_cnt[1] . $unique_udp_dst_port_cnt_info[2];
        if ($compact == 0) echo "</li></ul>";
        echo "</li></ul>";
    } else {
        echo "<table width='100%'><tr><td valign='top'>";
        if ($show_total_events) {
            $event_cnt = EventCnt($db, $join, $where);
            echo "<li>" . $event_cnt_info[0] . $event_cnt_info[1] . $event_cnt . $event_cnt_info[2] . "</li><li><p>";
        }
        //echo "<ul style='padding-left:20px'>";
        
?>
	  <table cellpadding=0 cellspacing=2 border=0 width="100%">
		<tr>
	  <?php
        //$li_style = (preg_match("/base_stat_sensor\.php/",$_SERVER['PHP_SELF'])) ? " style='color:#F37914'" : "";
        $color = (preg_match("/base_stat_sensor\.php/", $_SERVER['PHP_SELF'])) ? "#eeeeee" : "#D1D7DF";
        //echo "  <li$li_style>".$sensor_cnt_info[1]._SCSENSORS. "</a></li>";
        
?>
			<td nowrap align="center" bgcolor="<?php echo $color
?>"><?php echo $sensor_cnt_info[1] . _SCSENSORS . $sensor_cnt_info[2] ?></td>
	  <?php
        //$li_style = (preg_match("/base_stat_alerts\.php/",$_SERVER['PHP_SELF'])) ? " style='color:#F37914'" : "";
        $color = (preg_match("/base_stat_alerts\.php|base_stat_alerts_graph\.php/", $_SERVER['PHP_SELF'])) ? "#eeeeee" : "#D1D7DF";
        //echo "  <li$li_style>".$unique_alert_cnt_info[1]._UNIALERTS.$unique_alert_cnt_info[2] . "</li>";
        
?>
			<td nowrap align="center" bgcolor="<?php echo $color
?>"><?php echo $unique_alert_cnt_info[1] . _UNIALERTS . $unique_alert_cnt_info[2] ?> <a href="base_stat_alerts_graph.php"><img src="images/ico_graph.gif" align="absmiddle" border=0></a></td>
	  <?php
        if ($db->baseGetDBversion() >= 103) {
            //$li_style = (preg_match("/base_stat_class\.php/",$_SERVER['PHP_SELF'])) ? " style='color:#F37914'" : "";
            $color = (preg_match("/base_stat_class\.php|base_stat_class_graph\.php/", $_SERVER['PHP_SELF'])) ? "#eeeeee" : "#D1D7DF";
            //echo "<li$li_style>&nbsp;&nbsp;&nbsp;( ".$class_cnt_info[1]._SCCLASS."</a> )</li>";
            
?>
			<td nowrap align="center" bgcolor="<?php echo $color
?>">(<?php echo $class_cnt_info[1] . _SCCLASS . $class_cnt_info[2] ?>)</td>
	  <?php
        }
        //$li_style = (preg_match("/base_stat_iplink\.php/",$_SERVER['PHP_SELF'])) ? " style='color:#F37914'" : "";
        $color = (preg_match("/base_stat_iplink\.php/", $_SERVER['PHP_SELF'])) ? "#eeeeee" : "#D1D7DF";
        //echo "<li$li_style>".$unique_links_info[1].$unique_links_info[0].$unique_links_info[2]."</li>";
        
?>
			<td nowrap align="center" bgcolor="<?php echo $color
?>"><?php echo $unique_links_info[1] . $unique_links_info[0] . $unique_links_info[2] ?></td>
		</tr>
		<tr>
	  <?php
        //$li_style = (preg_match("/base_stat_uaddr\.php/",$_SERVER['PHP_SELF'])) ? " style='color:#F37914'" : "";
        $color = (preg_match("/base_stat_uaddr\.php/", $_SERVER['PHP_SELF'])) ? "#eeeeee" : "#D1D7DF";
        // echo "  <li$li_style>"._SCUNIADDRESS.
        //       $unique_src_ip_cnt_info[1]._SCSOURCE.' | '.$unique_src_ip_cnt_info[2].
        //       $unique_dst_ip_cnt_info[1]._SCDEST.$unique_dst_ip_cnt_info[2]."</li>";
        //echo "</td><td valign='top' style='padding-left:10px'>";
        $addrtype1 = ($_GET['addr_type'] == '1') ? "underline" : "none";
        $addrtype2 = ($_GET['addr_type'] == '2') ? "underline" : "none";
?>
			<td align="center" bgcolor="<?php echo $color
?>"><?php echo _SCUNIADDRESS . "<br>" . $unique_src_ip_cnt_info[1] . "<font style='text-decoration:$addrtype1'>" . _SCSOURCE . "</font>" . $unique_src_ip_cnt_info[2] . ' | ' . $unique_dst_ip_cnt_info[1] . "<font style='text-decoration:$addrtype2'>" . _SCDEST . "</font>" . $unique_dst_ip_cnt_info[2] ?></td>
	  <?php
        //$li_style = (preg_match("/base_stat_ports\.php/",$_SERVER['PHP_SELF'])) ? " style='color:#F37914'" : "";
        $color = (preg_match("/base_stat_ports\.php/", $_SERVER['PHP_SELF']) && $_GET['port_type'] == 1) ? "#eeeeee" : "#D1D7DF";
        //echo "<li$li_style>".$unique_src_port_cnt_info[1]._SCSOURCE." ".$unique_src_port_cnt_info[2]._SCPORT.": ".
        //       $unique_tcp_src_port_cnt_info[1]." TCP</a> | ".
        //       $unique_udp_src_port_cnt_info[1]." UDP</a>".
        //     "</li><li$li_style>".
        //       $unique_dst_port_cnt_info[1]._SCDEST." ".$unique_dst_port_cnt_info[2]._SCPORT.": ".
        //       $unique_tcp_dst_port_cnt_info[1]." TCP</a> | ".
        //       $unique_udp_dst_port_cnt_info[1]." UDP</a>" .
        //     "</li>";
        $sprototcp = ($_GET['proto'] == '6' && $_GET['port_type'] == '1') ? "underline" : "none";
        $sprotoudp = ($_GET['proto'] == '17' && $_GET['port_type'] == '1') ? "underline" : "none";
        $dprototcp = ($_GET['proto'] == '6' && $_GET['port_type'] == '2') ? "underline" : "none";
        $dprotoudp = ($_GET['proto'] == '17' && $_GET['port_type'] == '2') ? "underline" : "none";
?>
			<td align="center" bgcolor="<?php echo $color
?>"><?php echo $unique_src_port_cnt_info[1] . _SCSOURCE . " " . $unique_src_port_cnt_info[2] . _SCPORT . ":<br> " . $unique_tcp_src_port_cnt_info[1] . " <font style='text-decoration:$sprototcp'>TCP</font></a> | " . $unique_udp_src_port_cnt_info[1] . " <font style='text-decoration:$sprotoudp'>UDP</font></a>" ?></td>
      <?php
        $color = (preg_match("/base_stat_ports\.php/", $_SERVER['PHP_SELF']) && $_GET['port_type'] == 2) ? "#eeeeee" : "#D1D7DF";
?>
			<td align="center" bgcolor="<?php echo $color
?>"><?php echo $unique_dst_port_cnt_info[1] . _SCDEST . " " . $unique_dst_port_cnt_info[2] . _SCPORT . ":<br> " . $unique_tcp_dst_port_cnt_info[1] . " <font style='text-decoration:$dprototcp'>TCP</font></a> | " . $unique_udp_dst_port_cnt_info[1] . " <font style='text-decoration:$dprotoudp'>UDP</font></a>" ?></td> 
	  <?php
        if (!array_key_exists("minimal_view", $_GET)) {
            //$li_style = (preg_match("/base_stat_time\.php/",$_SERVER['PHP_SELF'])) ? " style='color:#F37914'" : "";
            $color = (preg_match("/base_stat_time\.php/", $_SERVER['PHP_SELF'])) ? "#eeeeee" : "#D1D7DF";
            //echo '<li><A '.$li_style.' HREF="base_stat_time.php">'._QSCTIMEPROF.'</A> '._QSCOFALERTS . "</li>";
            
?>
			<td align="center" bgcolor="<?php echo $color
?>"><?php echo '<A ' . $li_style . ' HREF="base_stat_time.php">' . _QSCTIMEPROF . '</A><br>' . _QSCOFALERTS ?></td>
	  <?php
        }
        //echo "</td></tr></table>";
        
?>
	  </tr>
	 </table>
	  <?php
        echo "</td></tr></table>";
    }
}
// plot graph
function plot_graphic($id, $height, $width, $xaxis, $yaxis, $xticks, $xlabel, $display = false) {
    $plot = '<script language="javascript" type="text/javascript">';
    $plot.= '$( function () {';
    $plot.= 'var options = { ';
    $plot.= 'lines: { show:true, labelHeight:0, lineWidth: 0.7},';
    $plot.= 'points: { show:false, radius: 2 }, legend: { show: false },';
    $plot.= 'yaxis: { ticks:[] }, xaxis: { tickDecimals:0, ticks: [';
    if (sizeof($xticks) > 0) {
        foreach($xticks as $k => $v) {
            $plot.= '[' . $v . ',"' . $xlabel[$k] . '"],';
        }
        $plot = preg_replace("/\,$/", "", $plot);
    }
    $plot.= ']},';
    $plot.= 'grid: { color: "#678297", labelMargin:0, backgroundColor: "#F0F0F0", tickColor: "#8F8F8F", hoverable:true, clickable:true}';
    $plot.= ', shadowSize:1 };';
    $plot.= 'var data = [{';
    $plot.= 'color: "rgb(18,55,95)", label: "Events", ';
    $plot.= 'lines: { show: true, fill: true},'; //$plot .= 'label: "Day",';
    $plot.= 'data:[';
    foreach($xaxis as $k => $v) {
        $plot.= '[' . $v . ',' . $yaxis[$k] . '],';
    }
    $plot = preg_replace("/\,$/", "]", $plot);
    $plot.= ' }];';
    $plot.= 'var plotarea = $("#' . $id . '");';
    if ($display == true) {
        $plot.= 'plotarea.css("display", "");';
        $width = '((window.innerWidth || document.body.clientWidth)/2)';
    }
    $plot.= 'plotarea.css("height", ' . $height . ');';
    $plot.= 'plotarea.css("width", ' . $width . ');';
    $plot.= '$.plot( plotarea , data, options );';
    //if ($display==true) {
    $plot.= 'var previousPoint = null;
			$("#' . $id . '").bind("plothover", function (event, pos, item) {
				if (item) {
					if (previousPoint != item.datapoint) {
						previousPoint = item.datapoint;
						$("#tooltip").remove();
						var x = item.datapoint[0].toFixed(2), y = item.datapoint[1].toFixed(0);
						showTooltip(item.pageX, item.pageY, y + " " + item.series.label);
					}
				}
				else {
					$("#tooltip").remove();
					previousPoint = null;
				}
			});';
    //}
    $plot.= '});</script>';
    return $plot;
}
// return arrays complete for time range
function range_graphic($timerange) {
    switch ($timerange) {
        case "today":
            $desde = strtotime(date("Y-m-d 00:00:00"));
            $suf = "h";
            $jump = 3600;
            $noprint = 2;
            $interval = "G";
            $key = "G";
            $hasta = time();
            break;

        case "day":
            $desde = strtotime("-23 hour");
            $suf = "";
            $jump = 3600;
            $noprint = 3;
            $interval = "G\h jM";
            $key = "G j";
            $hasta = time() + $jump;
            break;

        case "week":
            $desde = strtotime("-1 week");
            $suf = "";
            $jump = 86400;
            $noprint = 1;
            $interval = "j M";
            $key = "j F";
            $hasta = time();
            break;

        case "weeks":
            $desde = strtotime("-2 week");
            $suf = "";
            $jump = 86400;
            $noprint = 3;
            $interval = "j M";
            $key = "j F";
            $hasta = time();
            break;

        case "month":
            $desde = strtotime("-1 month");
            $suf = "";
            $jump = 86400;
            $noprint = 3;
            $interval = "j M";
            $key = "j F";
            $hasta = time();
            break;

        default:
            $desde = strtotime("-11 month");
            $suf = "";
            $jump = 0;
            $noprint = 2;
            $interval = "M-Y";
            $key = "F Y";
            $hasta = time();
    }
    //
    $x = $y = $ticks = $labels = array();
    $d = $desde;
    $xx = 0;
    while ($d <= $hasta) {
        $now = trim(date($key, $d) . " " . $suf);
        $x["$now"] = $ticks["$now"] = $xx++;
        $y["$now"] = 0; // default value 0
        $labels["$now"] = ($xx % $noprint == 0) ? date($interval, $d) . $suf : "";
        if ($jump == 0) $d+= (date("t", $d) * 86400); // case year
        else $d+= $jump; // next date
        
    }
    //print_r($labels);
    return array(
        $x,
        $y,
        $ticks,
        $labels
    );
}
?>
