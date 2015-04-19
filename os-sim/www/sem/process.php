<?php
/*****************************************************************************
*
*    License:
*
*   Copyright (c) 2003-2006 ossim.net
*   Copyright (c) 2007-2009 AlienVault
*   All rights reserved.
*
*   This package is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; version 2 dated June, 1991.
*   You may not use, modify or distribute this program under any other version
*   of the GNU General Public License.
*
*   This package is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this package; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
*   MA  02110-1301  USA
*
*
* On Debian GNU/Linux systems, the complete text of the GNU General
* Public License can be found in `/usr/share/common-licenses/GPL-2'.
*
* Otherwise you can read it here: http://www.gnu.org/licenses/gpl-2.0.txt
****************************************************************************/
/**
* Class and Function List:
* Function list:
* Classes list:
*/
require_once ("classes/Session.inc");
Session::logcheck("MenuControlPanel", "ControlPanelSEM");
require_once ("classes/Host.inc");
require_once ("process.inc");
require_once ('ossim_db.inc');
$config = parse_ini_file("everything.ini");
$a = $_GET["query"];
$offset = $_GET["offset"];
if (intval($offset) < 1) {
    $offset = 0;
}
$start = $_GET["start"];
$end = $_GET["end"];
$sort_order = $_GET["sort"];
$uniqueid = $_GET["uniqueid"];
$db = new ossim_db();
$conn = $db->connect();
if (preg_match("/(.*plugin_id!=)(\S+)(.*)/", $a, $matches) || preg_match("/(.*plugin_id=)(\S+)(.*)/", $a, $matches)) {
    $plugin_name = mysql_real_escape_string($matches[2]);
    $query = "select id from plugin where name like '" . $plugin_name . "%' order by id";
    if (!$rs = & $conn->Execute($query)) {
        print $conn->ErrorMsg();
        exit();
    }
    if ($plugin_id = $rs->fields["id"] != "") {
        $plugin_id = $rs->fields["id"];
    } else {
        $plugin_id = $matches[2];
    }
    $a = $matches[1] . $plugin_id . $matches[3];
}
if (preg_match("/(.*sensor!=)(\S+)(.*)/", $a, $matches) || preg_match("/(.*sensor=)(\S+)(.*)/", $a, $matches)) {
    $plugin_name = mysql_real_escape_string($matches[2]);
    $query = "select ip from sensor where name like '" . $plugin_name . "%'";
    if (!$rs = & $conn->Execute($query)) {
        print $conn->ErrorMsg();
        exit();
    }
    if ($plugin_id = $rs->fields["ip"] != "") {
        $plugin_id = $rs->fields["ip"];
    } else {
        $plugin_id = $matches[2];
    }
    $a = $matches[1] . $plugin_id . $matches[3];
}
$_SESSION["forensic_query"] = $a;
$_SESSION["forensic_start"] = $start;
$_SESSION["forensic_end"] = $end;
print "<center>";
$time1 = microtime(true);
$cmd = process($a, $start, $end, $offset, $sort_order, "logs", $uniqueid);
//$status = exec($cmd, $result);
$result = array();
$fp = popen("$cmd 2>>/dev/null", "r");
while (!feof($fp)) {
    $line = trim(fgets($fp));
    if ($line != "") $result[] = $line;
}
fclose($fp);
$time2 = microtime(true);
$totaltime = round($time2 - $time1, 2);
print " - Parsing time: <b>$totaltime</b> seconds. </center>";
//$num_lines = get_lines($a, $start, $end, $offset, $sort_order, "logs", $uniqueid);
$num_lines = count($result);
// Avoid graphs being drawn with more than 100000 events
if ($num_lines > 500000) {
?>
	<script>
	document.getElementById('too_many_events').style.display = 'block';
	document.getElementById('test').style.display = 'none';
	</script>
<?php
}
$alt = 0;
$end_lines = 50 + $offset;
if (($num_lines - $offset) < $end_lines) {
    $end_lines = $num_lines - $offset;
}
if ($end_lines > 50) {
    $end_lines = 50;
}
print "<center>\n";
if ($offset != 0) {
?>
<a href="javascript:DecreaseOffset(50);"><?php echo _("Previous 50") ?></a> | 
<?php
}
if ($num_lines > 50) { //if($num_lines > $offset + 50){
    
?>
<a href="javascript:IncreaseOffset(50);"><?php echo _("Next 50") ?></a>
<?php
}
print "</center>\n";
print "<table border='0' width='100%' cellpadding='2'>";
print "<tr><th>" . _("ID") . "</th><th>";
print "<a href=\"javascript:DateAsc()\"><font color=\"blue\"><</font></a>";
print " " . _("Date") . " ";
print "<a href=\"javascript:DateDesc()\"><font color=\"blue\">></font></a>";
print "</th><th>" . _("Event type");
print "</th><th>" . _("Host") . "</th><th>" . _("Source") . "</th><th>" . _("Dest") . "</th><th>" . _("Data") . "</th></tr>";
$color_words = array(
    'warning',
    'error',
    'failure',
    'break',
    'critical',
    'alert'
);
$inc_counter = 1 + $offset;
$cont = 0;
foreach($result as $res) if ($cont++ < 50) {
    $res = str_replace("<", "", $res);
    $res = str_replace(">", "", $res);
    //entry id='2' fdate='2008-09-19 09:29:17' date='1221816557' plugin_id='4004' sensor='192.168.1.99' src_ip='192.168.1.119' dst_ip='192.168.1.119' src_port='0' dst_port='0' data='Sep 19 02:29:17 ossim sshd[2638]: (pam_unix) session opened for user root by root(uid=0)'
    if (preg_match("/entry id='([^']+)'\s+fdate='([^']+)'\s+date='([^']+)'\s+plugin_id='([^']+)'\s+sensor='([^']+)'\s+src_ip='([^']+)'\s+dst_ip='([^']+)'\s+src_port='([^']+)'\s+dst_port='([^']+)'\s+tzone='[^']+'+\s+data='([^']+)'(\s+sig='([^']*)')?/", $res, $matches)) {
        $lf = explode(";", $res);
        $logfile = urlencode(end($lf));
        $data = $matches[10];
        $signature = $matches[12];
        $query = "select name from plugin where id = " . intval($matches[4]);
        if (!$rs = & $conn->Execute($query)) {
            print $conn->ErrorMsg();
            exit();
        }
        $plugin = htmlspecialchars($rs->fields["name"]);
        if ($plugin == "") {
            $plugin = intval($matches[4]);
        }
        $red = 0;
        $color = "black";
        $date = $matches[2];
        $sensor = $matches[5];
        $src_ip = $matches[6];
        $dst_ip = $matches[7];
        $src_port = $matches[8];
        $dst_port = $matches[9];
        $line = "<tr>
	<td nowrap>" . "<a href=\"../incidents/newincident.php?" . "ref=Alarm&" . "title=" . urlencode($plugin . " Event") . "&" . "priority=1&" . "src_ips=$src_ip&" . "event_end=$date&" . "src_ports=$src_port&" . "dst_ips=$dst_ip&" . "dst_ports=$dst_port" . "\">" . "<img src=\"../pixmaps/incident.png\" width=\"12\" alt=\"i\" border=\"0\"/></a> " . $inc_counter . "</td>
	<td nowrap>" . htmlspecialchars($matches[2]) . "</td>
	<td><font color=\"$color\"><span onmouseover=\"this.style.color = 'green'; this.style.cursor='pointer';\" onmouseout=\"this.style.color = '$color'; this.style.cursor = document.forms[0].cursor.value;\" onclick=\"javascript:SetSearch('plugin_id=' + this.innerHTML)\"\">$plugin</span></td>
	<td>";
        $line.= "<font color=\"$color\"><span onmouseover=\"this.style.color = 'green'; this.style.cursor='pointer';\" onmouseout=\"this.style.color = '$color';this.style.cursor = document.forms[0].cursor.value;\" onclick=\"javascript:SetSearch('src_ip=' + this.innerHTML)\"\">" . htmlspecialchars($matches[5]) . "</span></td><td>";
        $line.= "<font color=\"$color\"><span onmouseover=\"this.style.color = 'green'; this.style.cursor='pointer';\" onmouseout=\"this.style.color = '$color';this.style.cursor = document.forms[0].cursor.value;\" onclick=\"javascript:SetSearch('src_ip=' + this.innerHTML)\"\">" . htmlspecialchars($matches[6]) . "</span>:";
        $line.= "<font color=\"$color\"><span onmouseover=\"this.style.color = 'green'; this.style.cursor='pointer';\" onmouseout=\"this.style.color = '$color';this.style.cursor = document.forms[0].cursor.value;\" onclick=\"javascript:SetSearch('src_port=' + this.innerHTML)\"\">" . htmlspecialchars($matches[8]) . "</span></td><td>";
        $line.= "<font color=\"$color\"><span onmouseover=\"this.style.color = 'green'; this.style.cursor='pointer';\" onmouseout=\"this.style.color = '$color';this.style.cursor = document.forms[0].cursor.value;\" onclick=\"javascript:SetSearch('dst_ip=' + this.innerHTML)\"\">" . htmlspecialchars($matches[7]) . "</span>:";
        $line.= "<font color=\"$color\"><span onmouseover=\"this.style.color = 'green'; this.style.cursor='pointer';\" onmouseout=\"this.style.color = '$color';this.style.cursor = document.forms[0].cursor.value;\" onclick=\"javascript:SetSearch('dst_port=' + this.innerHTML)\"\">" . htmlspecialchars($matches[9]) . "</span></td>";
        if ($alt) {
            $color = "grey";
            $alt = 0;
        } else {
            $color = "blue";
            $alt = 1;
        }
        $verified = - 1;
        if ($signature != '') {
            $sig_dec = base64_decode($signature);
            $verified = 0;
            $pub_key = openssl_pkey_get_public($config["pubkey"]);
            $verified = openssl_verify($data, $sig_dec, $pub_key);
        }
        $data = $matches[10];
        $encoded_data = base64_encode($data);
        $data = "<td>";
        // change ,\s* or #\s* adding blank space to force html break line
        $matches[10] = preg_replace("/(\,|\#)\s*/", "\\1 ", $matches[10]);
        foreach(split("[\| \t;:]", $matches[10]) as $piece) {
            $clean_piece = str_replace("(", " ", $piece);
            $clean_piece = str_replace(")", " ", $clean_piece);
            $clean_piece = str_replace("[", " ", $clean_piece);
            $clean_piece = str_replace("]", " ", $clean_piece);
            $clean_piece = htmlspecialchars($piece);
            $red = 0;
            foreach($color_words as $word) {
                if (stripos($clean_piece, $word)) {
                    $red = 1;
                    break;
                }
            }
            if ($red) {
                $data.= "<font color=\"red\"><span onmouseover=\"this.style.color = 'green';this.style.cursor='pointer';\" onmouseout=\"this.style.color = 'red';this.style.cursor = document.forms[0].cursor.value;\" onclick=\"javascript:SetSearch('" . $clean_piece . "')\"\">" . $clean_piece . " </span>";
            } else {
                $data.= "<font color=\"$color\"><span onmouseover=\"this.style.color = 'green';this.style.cursor='pointer';\" onmouseout=\"this.style.color = '$color';this.style.cursor = document.forms[0].cursor.value;\" onclick=\"javascript:SetSearch('" . $clean_piece . "')\"\">" . $clean_piece . " </span>";
            }
        }
        if ($verified >= 0) {
            if ($verified == 1) {
                $data.= '<img src="' . $config["verified_graph"] . '" height=15 width=15 alt="V" />';
            } else if ($verified == 0) {
                $data.= '<img src="' . $config["failed_graph"] . '" height=15 width=15 alt="F" />';
            } else {
                $data.= '<img src="' . $config["error_graph"] . '" height=15 width=15 alt="E" />';
                $data.= openssl_error_string();
            }
        }
        $data.= '<a href="validate.php?log=' . $encoded_data . "&start=$start&end=$end&logfile=$logfile" . '" class="thickbox" rel="AjaxGroup" target="_blank"> <small>(Validate signature)</small></a>';
        $data.= "</td>";
        $line.= $data;
        $inc_counter++;
    }
    print $line;
}
print "</table>";
if ($num_lines == 0) {
    print "<center><font style='color:red;font-size:14px'><br>No Data Found Matching Your Criteria</center>";
}
print "<center>\n";
if ($offset != 0) {
?>
<a href="javascript:DecreaseOffset(50);"><?php echo _("Previous 50") ?></a> | 
<?php
}
if ($num_lines > $offset + 50) {
?>
<a href="javascript:IncreaseOffset(50);"><?php echo _("Next") . " " . $end_lines ?></a>
<?php
}
?>
</center>
