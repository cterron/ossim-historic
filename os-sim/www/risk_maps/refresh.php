<?
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
* - check_writable_relative()
* Classes list:
*/
require_once 'classes/Session.inc';
Session::logcheck("MenuControlPanel", "BusinessProcesses");

require_once 'ossim_db.inc';
require_once 'classes/Security.inc';

$map = $_GET["map"];

ossim_valid($map, OSS_DIGIT, OSS_ALPHA, ".",'illegal:'._("map"));

if (ossim_error()) {
die(ossim_error());
}

$db = new ossim_db();
$conn = $db->connect();
$params = array($map);
$query = "select * from risk_indicators where name <> 'rect' AND map= ? ";

if (!$rs = &$conn->Execute($query, $params)) {
	print $conn->ErrorMsg();
} else {
	while (!$rs->EOF){
		$name = $rs->fields["type_name"];
		$type = $rs->fields["type"];
		$host_types = array("host", "server", "sensor");
		// r --> bad
		// a --> medium
		// v --> good
		$RiskValue = 'v';
		$VulnValue = 'v';
		$AvailValue = 'v';

		$what = "name";

 		if(in_array($type, $host_types)){
    	if($type == "host"){                        
				$what = "hostname";                
			}
 			$query = "select ip from $type where $what = ?";
 			$params = array($name);
   		if ($rs3 = &$conn->Execute($query, $params)) {
      	$name = $rs3->fields["ip"];
      }
    }

		$params = array($name);

		if(in_array($type, $host_types)){
			$query = "select severity from bp_member_status where member = ? and measure_type = \"host_metric\"";
		} else {
			$query = "select severity from bp_member_status where member = ? and measure_type = \"net_metric\"";
		}

    if (!$rs2 = &$conn->Execute($query, $params)) {
    	print $conn->ErrorMsg();
		} else {
			if(intval($rs2->fields["severity"]) > 7){
				$RiskValue = 'r';
			} elseif(intval($rs2->fields["severity"]) > 3){
				$RiskValue = 'a';
			}
		}

		if(in_array($type, $host_types)){
			$query = "select severity from bp_member_status where member = ? and measure_type = \"host_vulnerability\"";
		} else {
			$query = "select severity from bp_member_status where member = ? and measure_type = \"net_vulnerability\"";
		}
    if (!$rs2 = &$conn->Execute($query, $params)) {
    	print $conn->ErrorMsg();
		} else {
			if(intval($rs2->fields["severity"]) > 7){
				$VulnValue = 'r';
			} elseif(intval($rs2->fields["severity"]) > 3){
				$VulnValue = 'a';
			}
		}

		if(in_array($type, $host_types)){
		$query = "select severity from bp_member_status where member = ? and measure_type = \"host_availability\"";
        	if (!$rs2 = &$conn->Execute($query, $params)) {
            	print $conn->ErrorMsg();
		} else {
			if(intval($rs2->fields["severity"]) > 7){
				$AvailValue = 'r';
			} elseif(intval($rs2->fields["severity"]) > 3){
				$AvailValue = 'a';
			}
			}
		}

		$new_value = "txt".$RiskValue.$VulnValue.$AvailValue;
		$change_div = "changeDiv('".$rs->fields["id"]."','".$rs->fields["name"]."','".$rs->fields["url"]."','".$rs->fields["icon"]."',$new_value);\n";
		echo $change_div;
    $rs->MoveNext();
		}
	}
	$conn->close();
?>
