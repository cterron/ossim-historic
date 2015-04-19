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
* - valid_value()
* - submit()
* Classes list:
*/
require_once 'classes/Session.inc';
Session::logcheck("MenuConfiguration", "ConfigurationMain");
require_once 'ossim_conf.inc';
require_once 'classes/Security.inc';
$ossim_conf = $GLOBALS["CONF"];
$updates_file = "/etc/ossim/updates/update_log.txt";
$CONFIG = array(
    "Updates" => array(
        "title" => gettext("Updates") ,
        "desc" => gettext("Configure updates") ,
        "advanced" => 0,
        "conf" => array(
            "update_checks_enable" => array(
                "type" => array(
                    "yes" => _("Yes") ,
                    "no" => _("No")
                ) ,
                "help" => gettext("The system will check once a day for updated packages, rules, directives, etc. No system information will be sent, it just gest a file with dates and update messages using wget.") ,
                "desc" => gettext("Enable auto update-checking") ,
                "advanced" => 0
            ) ,
            "update_checks_use_proxy" => array(
                "type" => array(
                    "yes" => _("Yes") ,
                    "no" => _("No")
                ) ,
                "help" => gettext("") ,
                "desc" => gettext("Use proxy for auto update-checking") ,
                "advanced" => 1
            ) ,
            "proxy_url" => array(
                "type" => "text",
                "help" => gettext("Enter the full path including a trailing slash, i.e., 'http://192.168.1.60:3128/'") ,
                "desc" => gettext("Proxy url") ,
                "advanced" => 1
            ) ,
            "proxy_user" => array(
                "type" => "text",
                "help" => gettext("") ,
                "desc" => gettext("Proxy User") ,
                "advanced" => 1
            ) ,
            "proxy_password" => array(
                "type" => "password",
                "help" => gettext("") ,
                "desc" => gettext("Proxy Password") ,
                "advanced" => 1
            ) ,
            "last_update" => array(
                "type" => "text",
                "help" => gettext("") ,
                "desc" => gettext("Last update timestamp") ,
                "advanced" => 1
            ) ,
        )
    )
);
function valid_value($key, $value) {
    $numeric_values = array(
        "recovery",
        "threshold",
        "use_resolv",
        "have_scanmap3d",
        "max_event_tmp"
    );
    if (in_array($key, $numeric_values)) {
        if (!is_numeric($value)) {
            require_once ("ossim_error.inc");
            $error = new OssimError();
            $error->display("NOT_NUMERIC", array(
                $key
            ));
        }
    }
    return true;
}
function submit() {
?>
    <!-- submit -->
    
    <input type="submit" class="btn" style="font-size:12px" value=" <?php
    echo gettext("Save Configuration"); ?> " />
	<br><br>
    <input type="button" onclick="lastupdate(this.form)" class="btn" style="font-size:120%;font-weight:bold" value=" <?php
    echo gettext("Acknowledge Updates"); ?> " />
	<br>
    <!-- end sumbit -->
<?php
}
if (POST('update')) {
    require_once 'classes/Config.inc';
    $config = new Config();
    for ($i = 0; $i < POST('nconfs'); $i++) {
        if (valid_value(POST("conf_$i") , POST("value_$i"))) {
            if (!$ossim_conf->is_in_file(POST("conf_$i"))) {
                $config->update(POST("conf_$i") , POST("value_$i"));
                //echo POST("conf_$i")."=".POST("value_$i");
                
            }
        }
    }
    header("Location: " . $_SERVER['SCRIPT_NAME']);
    exit;
}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
  <title> <?php
echo gettext("Updates"); ?> </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
  <style type="text/css">
	.semiopaque { opacity:0.9; MozOpacity:0.9; KhtmlOpacity:0.9; filter:alpha(opacity=90); background-color:#B5C3CF }
  </style>
  <script>
	var IE = document.all ? true : false
	if (!IE) document.captureEvents(Event.MOUSEMOVE)
	document.onmousemove = getMouseXY;
	var tempX = 0
	var tempY = 0

	var difX = 15
	var difY = 0 

	function getMouseXY(e) {
		if (IE) { // grab the x-y pos.s if browser is IE
				tempX = event.clientX + document.body.scrollLeft + difX
				tempY = event.clientY + document.body.scrollTop + difY 
		} else {  // grab the x-y pos.s if browser is MOZ
				tempX = e.pageX + difX
				tempY = e.pageY + difY
		}  
		if (tempX < 0){tempX = 0}
		if (tempY < 0){tempY = 0}
		var dh = document.body.clientHeight+ window.scrollY;
		if (document.getElementById("numeroDiv").offsetHeight+tempY > dh)
			tempY = tempY - (document.getElementById("numeroDiv").offsetHeight + tempY - dh)
		document.getElementById("numeroDiv").style.left = tempX
		document.getElementById("numeroDiv").style.top = tempY 
		return true
	}
	
	function ticketon(name,desc) { 
		if (document.getElementById) {
			var txt1 = '<table border=0 cellpadding=8 cellspacing=0 class="semiopaque"><tr><td class=nobborder style="line-height:18px;width:300px" nowrap><b>'+ name +'</b><br>'+ desc +'</td></tr></table>'
			document.getElementById("numeroDiv").innerHTML = txt1
			document.getElementById("numeroDiv").style.display = ''
			document.getElementById("numeroDiv").style.visibility = 'visible'
		}
	}

	function ticketoff() {
		if (document.getElementById) {
			document.getElementById("numeroDiv").style.visibility = 'hidden'
			document.getElementById("numeroDiv").style.display = 'none'
			document.getElementById("numeroDiv").innerHTML = ''
		}
	}
	
	function lastupdate(f) {
		f.value_5.value=f.last.value;
		f.value_5.disabled = false;
		f.submit();
	}
</script>

</head>
<body>
  <div id="numeroDiv" style="position:absolute; z-index:999; left:0px; top:0px; height:80px; visibility:hidden; display:none"></div>
  <?php
include ("../hmenu.php"); ?>
  
  <form method="POST" style="margin:0 auto" action="<?php
echo $_SERVER["SCRIPT_NAME"] ?>" />
  
  <table align=center>
  <tr>
  <td valign=top>
  
<?php
$count = 0;
$div = 0;
$found = 0;
$advanced = 1;
$arr = array();
foreach($CONFIG as $key => $val) if ($advanced || (!$advanced && $val["advanced"] == 0)) {
    $s = (POST('word') != "") ? POST('word') : ((GET('word') != "") ? GET('word') : "");
    if ($s != "") {
        foreach($val["conf"] as $conf => $type) if ($advanced || (!$advanced && $type["advanced"] == 0)) {
            if (preg_match("/$s/i", $conf)) {
                $found = 1;
                array_push($arr, $conf);
            }
        }
    }
?>
	<table width="100%" cellspacing="0" class=noborder>
		<th  <?php
    if ($found == 1) echo "style='background-color: #F28020; color: #FFFFFF'" ?>>
			<?php echo $val["desc"] ?>
		</th>
	</table>
	<table cellpadding=3 align="center" class=noborder>
<?php
    //print "<tr><th colspan=\"2\">" . $val["title"] . "</th></tr>";
    foreach($val["conf"] as $conf => $type) if ($advanced || (!$advanced && $type["advanced"] == 0)) {
        //var_dump($type["type"]);
        $conf_value = $ossim_conf->get_conf($conf);
        $var = ($type["desc"] != "") ? $type["desc"] : $conf;
?>
    <tr <?php
        if (in_array($conf, $arr)) echo "bgcolor=#FE9B52" ?>>

      <input type="hidden" name="conf_<?php
        echo $count ?>"
             value="<?php
        echo $conf ?>" />

      <td><b><?php echo $var ?></b></td>
      <td class="left">
<?php
        $input = "";
        $disabled = ($type["disabled"] == 1 || $ossim_conf->is_in_file($conf)) ? "class=\"disabled\" disabled" : "";
        /* select */
        if (is_array($type["type"])) {
            $input.= "<select name=\"value_$count\" $disabled>";
            if ($conf_value == "") $input.= "<option value=''>";
            foreach($type["type"] as $option_value => $option_text) {
                $input.= "<option ";
                if ($conf_value == $option_value) $input.= " SELECTED ";
                $input.= "value=\"$option_value\">$option_text</option>";
            }
            $input.= "</select>";
        }
        /* input */
        else {
            $input.= "<input ";
            //if ($ossim_conf->is_in_file($conf)) {
            //   $input .= " class=\"disabled\" ";
            //    $input .= " DISABLED ";
            //}
            $input.= "type=\"" . $type["type"] . "\" size=\"30\" 
                    name=\"value_$count\" value=\"$conf_value\" $disabled/>";
        }
        echo $input;
?>
      </td><td align="left"><a href="javascript:;" onmouseover="ticketon('<?php echo str_replace("'", "\'", $var) ?>','<?php echo str_replace("'", "\'", $type["help"]) ?>')"  onmouseout="ticketoff()"><img src="../pixmaps/help.png" width="16" border=0></a></td>

    </tr>
<?php
        $count+= 1;
    }
?>
    <tr>
		<td align=center colspan=3><?php
    submit(); ?></td>
	</tr>
	</table>

<?php
    $found = 0;
}
?>
  
  </td><td width=10></td>
  <td valign=top width="60%">
	<table width="100%" cellspacing="0" class=noborder>
		<th  <?php
if ($found == 1) echo "style='background-color: #F28020; color: #FFFFFF'" ?>>
			<?php echo _("Latest Updates") ?>
		</th>
	</table>
	<table cellpadding=3 align="center" class=noborder>
	<?php
$conf_value = strtotime($ossim_conf->get_conf("last_update"));
$i = $timeupdate = 0;
if (file_exists($updates_file)) {
    $updatesf = array_reverse(file($updates_file));
    foreach($updatesf as $line) if (preg_match("/^(\d+)\s(.*)/", trim($line) , $found) != "") {
        $time = strtotime($found[1]);
        if ($i++ == 0) $timeupdate = $time;
        $color = ($time > $conf_value) ? "#DF0033" : "black";
?>
		<tr>
			<td style="color:<?php echo $color
?>" width="80"><?php echo date("Y-m-d", $time) ?></td>
			<td style="text-align:left;color:<?php echo $color ?>"><?php echo $found[2] ?></td>
		</tr>
<?php
    }
}
?>		
	</table>
 </td>
</tr>
</table>
  <input type="hidden" name="update" value="yes" />
  <input type="hidden" name="last" value="<?php echo date("Y-m-d", $timeupdate) ?>" />
  <input type="hidden" name="nconfs" value="<?php
echo $count ?>" />

  </form>

</body>
</html>

