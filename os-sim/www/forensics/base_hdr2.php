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
$today_d = date("d");
$today_m = date("m");
$today_y = date("Y");
$today_h = date("h");
//$yesterday_d = date("d",mktime(0,0,0, $today_m, $today_d - 1, $today_y));
//$yesterday_m = date("m",mktime(0,0,0, $today_m, $today_d - 1, $today_y));
//$yesterday_y = date("Y",mktime(0,0,0, $today_m, $today_d - 1, $today_y));
$yesterday_d = date("d", strtotime("-1 day"));
$yesterday_m = date("m", strtotime("-1 day"));
$yesterday_y = date("Y", strtotime("-1 day"));
//$week_d = date("d",mktime(0,0,0, $today_m, $today_d - (date("w") +1), $today_y));
//$week_m = date("m",mktime(0,0,0, $today_m, $today_d - (date("w") +1), $today_y));
//$week_y = date("Y",mktime(0,0,0, $today_m, $today_d - (date("w") +1), $today_y));
$week_d = date("d", strtotime("-1 week"));
$week_m = date("m", strtotime("-1 week"));
$week_y = date("Y", strtotime("-1 week"));
//$two_week_d = date("d",mktime(0,0,0, $today_m, $today_d - 7 - (date("w") +1), $today_y));
//$two_week_m = date("m",mktime(0,0,0, $today_m, $today_d - 7 -  (date("w") +1), $today_y));
//$two_week_y = date("Y",mktime(0,0,0, $today_m, $today_d - 7 -  (date("w") +1), $today_y));
$two_week_d = date("d", strtotime("-2 week"));
$two_week_m = date("m", strtotime("-2 week"));
$two_week_y = date("Y", strtotime("-2 week"));
//$month_d = date("d",mktime(0,0,0, $today_m, 1, $today_y));
//$month_m = date("m",mktime(0,0,0, $today_m, 1, $today_y));
//$month_y = date("Y",mktime(0,0,0, $today_m, 1, $today_y));
$month_d = date("d", strtotime("-1 month"));
$month_m = date("m", strtotime("-1 month"));
$month_y = date("Y", strtotime("-1 month"));
//$two_month_d = date("d",mktime(0,0,0, $today_m - 1, 1, $today_y));
//$two_month_m = date("m",mktime(0,0,0, $today_m - 1, 1, $today_y));
//$two_month_y = date("Y",mktime(0,0,0, $today_m - 1, 1, $today_y));
$two_month_d = date("d", strtotime("-2 month"));
$two_month_m = date("m", strtotime("-2 month"));
$two_month_y = date("Y", strtotime("-2 month"));
//$year_d = date("d",mktime(0,0,0, 1, 1, $today_y));
//$year_m = date("m",mktime(0,0,0, 1, 1, $today_y));
//$year_y = date("Y",mktime(0,0,0, 1, 1, $today_y));
$year_d = date("d", strtotime("-11 month"));
$year_m = date("m", strtotime("-11 month"));
$year_y = date("Y", strtotime("-11 month"));
//$two_year_d = date("d",mktime(0,0,0, 1, 1, $today_y-1));
//$two_year_m = date("m",mktime(0,0,0, 1, 1, $today_y-1));
//$two_year_y = date("Y",mktime(0,0,0, 1, 1, $today_y-1));
$two_year_d = date("d", strtotime("-2 year"));
$two_year_m = date("m", strtotime("-2 year"));
$two_year_y = date("Y", strtotime("-2 year"));
$sensor = ($_GET["sensor"] != "") ? $_GET["sensor"] : $_SESSION["sensor"];
$sterm = ($_GET['search_str'] != "") ? $_GET['search_str'] : ($_SESSION['search_str'] != "" ? $_SESSION['search_str'] : "search term");
$risk = ($_GET["ossim_risk_a"] != "") ? $_GET["ossim_risk_a"] : $_SESSION["ossim_risk_a"];
?>
<!-- MAIN HEADER TABLE -->
<table width='100%' border='0' align="center" class="headermenu"><tr><td valign="top" width="380">
	
<form name="QueryForm" id="frm" ACTION="base_qry_main.php" method="GET" style="margin:0 auto">
<input type='hidden' name="search" value="1" />
<input type="hidden" name="sensor" id="sensor" value="<?php echo $sensor
?>" />
<input type="submit" name="bsf" id="bsf" value="QueryDB" style="display:none">

<!--<input type='hidden' name="saved_get" value='<?php
//= serialize($_GET)
 ?>'>-->
<table width='100%' border='0' align="center">
<tr>
	<td>
		<table width='100%'>
			<tr>
				<td class='menuitem' nowrap>
				<a class='menuitem' href='<?php echo $BASE_urlpath ?>/base_main.php'><font style="font-size:14px"><?php echo _HOME ?></font></a>&nbsp;&nbsp;|&nbsp;&nbsp;
				<a class='menuitem' href='<?php echo $BASE_urlpath ?>/base_qry_main.php?new=1'><font style="font-size:14px"><?php echo _SEARCH ?></font></a>&nbsp;&nbsp;
				<?php
if ($Use_Auth_System == 1) {
?>
				|&nbsp;&nbsp;<a class='menuitem' href='<?php echo $BASE_urlpath
?>/base_user.php'><?php echo _USERPREF
?></a>
				&nbsp;&nbsp;|&nbsp;&nbsp;<a class='menuitem' href='<?php echo $BASE_urlpath
?>/base_logout.php'><?php echo _LOGOUT
?></a>
				<?php
}
?>
				</td>
				<!--
				<TD class="menuitem"><FONT color="#8D4102"><B>Cached:&nbsp&nbsp</B></FONT>
			        <A class="menuitem" href="base_stat_alerts.html">Uniq</A> &nbsp&nbsp|&nbsp&nbsp
			        <A class="menuitem" href="base_stat_uaddr1.html">Src</A> &nbsp&nbsp|&nbsp&nbsp
			        <A class="menuitem" href="base_stat_uaddr2.html">Dst</A> &nbsp&nbsp|&nbsp&nbsp
			        <A class="menuitem" href="base_stat_ports2.html">Dst Port</A>
			    </td>-->
				<td align="right">
					<table border=0 cellpadding=0 cellspacing=0>
					<tr>
						<td>
							<table width="100%">
								<tr>			
									<td>[<?php echo $back_link
?>  <a href="base_qry_main.php?submit=<?php echo _QUERYDB
?>">Return to Main</a> &nbsp;|&nbsp; <a class='menuitem' href='<?php echo $BASE_urlpath
?>/base_maintenance.php'>Administration</a> ]</td>
									<TD></TD>
								</tr>
							</table>
						</td>
					</tr>			
					</table>						
				</td>
			</tr>
		</table>
	</td>
</tr>
<tr>
	<td>
		<table border=0 cellpadding=0 cellspacing=0>
			<tr><td nowrap>
				<table>
				<tr>
				<td>Sensor </td>
				<td>Plugin </td>
				<td>Risk </td>
				</tr>
				<tr>
				<td><select name="ip" id="ip" class="selectp" style="width:155px" onchange="this.form.sensor.value=this.options[this.selectedIndex].value;this.form.bsf.click()"><option value=''></option>
				<?php
					$snortsensors = GetSensorSids($db);
					$sns = array();
					foreach($snortsensors as $ip => $sids) {
						$sid = implode(",", $sids);
						$sname = ($sensors[$ip] != "") ? $sensors[$ip] : (($hosts[$ip] != "") ? $hosts[$ip] : "");
						$sns[$sname] = array($ip,$sid);
					}
					// sort by sensor name
					ksort($sns);
					foreach ($sns as $sname => $ip) {
						$sel = ($ip[1] != "" && $sensor == $ip[1]) ? "selected" : "";
						$ip[0] .= ($sname != "") ? " [" . $sname . "]" : "";
						echo "<option value='".$ip[1]."' $sel>".$ip[0]."</option>\n";
					}
				?>
				</select></td>
				<td><select name="plugin" class="selectp" style="width:130px" onchange="this.form.sensor.value=this.options[this.selectedIndex].value;this.form.bsf.click()"><option value=''></option>
				<?php
$snortsensors = GetSensorPluginSids($db);
foreach($snortsensors as $plugin => $sids) {
    $sid = implode(",", $sids);
    $sel = ($sid != "" && $sensor == $sid) ? "selected" : "";
    //$id_plugin = $plugins[$plugin];
    echo "<option value='$sid' $sel>$plugin</option>\n";
}
?>
				</select></td>
				<td><select name="ossim_risk_a" class="selectp" style="width:70px" onchange="this.form.bsf.click()"><option value=' '>
				<option value="low"<?php
if ($risk == "low") echo " selected" ?>>Low
				<option value="medium"<?php
if ($risk == "medium") echo " selected" ?>>Medium
				<option value="high"<?php
if ($risk == "high") echo " selected" ?>>High
				</select></td>
				</tr></table>
			</td></tr>
			<tr><td>
				<table><tr>
				<td><input type="text" name="search_str" style="width:200px" value="<?php echo $sterm ?>" onfocus="if(this.value=='search term') this.value=''"></td>
				<td><img src="images/help.gif" border=0 alt="You can use +,-,* modifiers" title="You can use +,-,* modifiers"></td>
				<td colspan=2><input type="submit" class="button" value="Signature" name="submit">&nbsp;<input type="submit" class="button" value="Payload" name="submit"></td>
				</tr></table>
			</td></tr>
		</table>
	</td>
</tr>
<tr>
	<td>
		<table>
			<tr>
				<td>Time frame selection:</td>
				<?php
$urltimecriteria = $_SERVER['PHP_SELF'];
$params = "";
// Clicked from qry_alert or clicked from Time profile must return to main
if (preg_match("/base_qry_alert|base_stat_time/", $urltimecriteria)) {
    $urltimecriteria = "base_qry_main.php";
}
if ($_GET["addr_type"] != "") $params.= "&addr_type=" . $_GET["addr_type"];
if ($_GET["sort_order"] != "") $params.= "&sort_order=" . $_GET["sort_order"];
//print_r($_GET);

?>
			</tr>
			<tr>
				<td nowrap>
					<a <?php
if ($_GET['time_range'] == "today") echo "style='text-decoration:underline;font-weight:bold'" ?> href="<?php echo $urltimecriteria ?>?time_range=today&new=1&time%5B0%5D%5B0%5D=+&time%5B0%5D%5B1%5D=%3E%3D&time%5B0%5D%5B2%5D=<?php echo $today_m ?>&time%5B0%5D%5B3%5D=<?php echo $today_d ?>&time%5B0%5D%5B4%5D=<?php echo $today_y ?>&time%5B0%5D%5B5%5D=&time%5B0%5D%5B6%5D=&time%5B0%5D%5B7%5D=&time%5B0%5D%5B8%5D=+&time%5B0%5D%5B9%5D=+&submit=Query+DB&num_result_rows=-1&time_cnt=1<?php echo $params ?>"> Today </a>
					|
					<a <?php
if ($_GET['time_range'] == "day") echo "style='text-decoration:underline;font-weight:bold'" ?> href="<?php echo $urltimecriteria ?>?time_range=day&new=1&time%5B0%5D%5B0%5D=+&time%5B0%5D%5B1%5D=%3E%3D&time%5B0%5D%5B2%5D=<?php echo $yesterday_m ?>&time%5B0%5D%5B3%5D=<?php echo $yesterday_d ?>&time%5B0%5D%5B4%5D=<?php echo $yesterday_y ?>&time%5B0%5D%5B5%5D=<?php echo $today_h ?>&time%5B0%5D%5B6%5D=&time%5B0%5D%5B7%5D=&time%5B0%5D%5B8%5D=+&time%5B0%5D%5B9%5D=+&submit=Query+DB&num_result_rows=-1&time_cnt=1<?php echo $params ?>">Last 24 Hours</a>
					|
					<a <?php
if ($_GET['time_range'] == "week") echo "style='text-decoration:underline;font-weight:bold'" ?> href="<?php echo $urltimecriteria ?>?time_range=week&new=1&time%5B0%5D%5B0%5D=+&time%5B0%5D%5B1%5D=%3E%3D&time%5B0%5D%5B2%5D=<?php echo $week_m ?>&time%5B0%5D%5B3%5D=<?php echo $week_d ?>&time%5B0%5D%5B4%5D=<?php echo $week_y ?>&time%5B0%5D%5B5%5D=&time%5B0%5D%5B6%5D=&time%5B0%5D%5B7%5D=&time%5B0%5D%5B8%5D=+&time%5B0%5D%5B9%5D=+&submit=Query+DB&num_result_rows=-1&time_cnt=1<?php echo $params ?>">Last Week</a>
					|
					<a <?php
if ($_GET['time_range'] == "weeks") echo "style='text-decoration:underline;font-weight:bold'" ?> href="<?php echo $urltimecriteria ?>?time_range=weeks&new=1&time%5B0%5D%5B0%5D=+&time%5B0%5D%5B1%5D=%3E%3D&time%5B0%5D%5B2%5D=<?php echo $two_week_m ?>&time%5B0%5D%5B3%5D=<?php echo $two_week_d ?>&time%5B0%5D%5B4%5D=<?php echo $two_week_y ?>&time%5B0%5D%5B5%5D=&time%5B0%5D%5B6%5D=&time%5B0%5D%5B7%5D=&time%5B0%5D%5B8%5D=+&time%5B0%5D%5B9%5D=+&submit=Query+DB&num_result_rows=-1&time_cnt=1<?php echo $params ?>">Last two Weeks</a>
					|
					<a <?php
if ($_GET['time_range'] == "month") echo "style='text-decoration:underline;font-weight:bold'" ?> href="<?php echo $urltimecriteria ?>?time_range=month&new=1&time%5B0%5D%5B0%5D=+&time%5B0%5D%5B1%5D=%3E%3D&time%5B0%5D%5B2%5D=<?php echo $month_m ?>&time%5B0%5D%5B3%5D=<?php echo $month_d ?>&time%5B0%5D%5B4%5D=<?php echo $month_y ?>&time%5B0%5D%5B5%5D=&time%5B0%5D%5B6%5D=&time%5B0%5D%5B7%5D=&time%5B0%5D%5B8%5D=+&time%5B0%5D%5B9%5D=+&submit=Query+DB&num_result_rows=-1&time_cnt=1<?php echo $params ?>">Last Month</a>
					|
					<a <?php
if ($_GET['time_range'] == "all") echo "style='text-decoration:underline;font-weight:bold'" ?> href="<?php echo $urltimecriteria ?>?time_range=all&clear_criteria=time&clear_criteria_element=&new=1&submit=Query+DB<?php echo $params ?>">All</a>
			    </td>
			</tr>
			<tr>
				<td id="task" style="display:none" nowrap>
					<div class="balloon">
						<a href="#"><img src="images/alarm-clock-blue.png" align="absmiddle" border=0> <i> Background task in process</i></a>
						<span class="tooltip">
								<span class="top"></span>
								<span class="middle" id="bgtask"><?php echo _("No pending tasks") ?>.</span>
								<span class="bottom"></span>
						</span>
					</div> 
				</td>
			</tr>
		</table>
	</td>
</tr>
<!--
<tr>
	<td><?php
//PrintFramedBoxHeader(_QSCSUMM, "#669999", "#FFFFFF");
//PrintGeneralStats($db, 1, $show_summary_stats, "$join_sql ", "$where_sql $criteria_sql");

?></td>
</tr>
<tr>
	<td>
		<table width="100%">
			<tr>
				<td width="250" nowrap><B><?php echo _QUERIED
?></B><FONT> : <?php echo strftime(_STRFTIMEFORMAT) ?></FONT></td>
				<td width="130" nowrap><div id="forensics_time"></div></td>
			</tr>
		</table>
	</td>
</tr>
-->
</table>

</form>


</td><td valign="top">
<link href="styles/combo.css" rel="stylesheet" type="text/css" />
<link href="styles/skin.css" rel="stylesheet" type="text/css" />
<script src="js/jquery-1.3.2.min.js" type="text/javascript"></script>
<script src="js/jquery.sexy-combo-2.0.6.js" type="text/javascript"></script>
<script src="js/jquery.flot.pack.js" language="javascript" type="text/javascript"></script>
<?php
if (isset($_SERVER['HTTP_USER_AGENT']) && (strpos($_SERVER['HTTP_USER_AGENT'], 'MSIE') !== false)) echo '<script src="js/excanvas.pack.js" language="javascript" type="text/javascript" ></script>';
?>
<script>
	function showTooltip(x, y, contents) {
		$('<div id="tooltip" class="tooltipLabel">' + contents + '</div>').css( {
			position: 'absolute',
			display: 'none',
			top: y + 5,
			left: x + 8,
			border: '1px solid #ADDF53',
			padding: '1px 2px 1px 2px',
			'background-color': '#CFEF95',
			opacity: 0.80
		}).appendTo("body").fadeIn(200);
	}
	//$(document).ready(function(){
	    $("#ip").sexyCombo({
			triggerSelected: true,
			textChangeCallback: function() {
				var t = this.getTextValue();
				$('#ip option').each(function(){
					var str = $(this).html()
					str = str.replace(/^\s*|\n|\t|\s*$/g,"");
					if (str == t && $(this).val()!='<?php echo $sensor
?>') {
						$("#sensor").val($(this).val());
						$("#bsf").click();
					}
				});
			}
		});
	//});
	function bgtask() {
		$.ajax({
			type: "GET",
			url: "base_bgtask.php",
			data: "",
			success: function(msg) {
				if (msg.match(/No pending tasks/)) {
					$("#bgtask").html(msg);
					if ($("#task").is(":visible")) $("#task").toggle();
				} else {
					if ($("#task").is(":hidden")) $("#task").toggle();
					$("#bgtask").html(msg);
					setInterval("bgtask()",10000);
				}
			}
		});
	}
	<?php
if ($_SESSION["deletetask"] != "") echo "bgtask();\n"; ?>
</script>
<div>
