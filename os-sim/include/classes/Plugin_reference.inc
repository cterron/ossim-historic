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
* - Plugin_reference()
* - get_plugin_id()
* - get_plugin_sid()
* - get_reference_id()
* - get_reference_sid()
* - get_count()
* - get_list()
* Classes list:
* - Plugin_reference
*/
require_once ("ossim_db.inc");
class Plugin_reference {
    var $plugin_id;
    var $plugin_sid;
    var $reference_id;
    var $reference_sid;
    function Plugin_reference($plugin_id, $plugin_sid, $reference_id, $reference_sid) {
        $this->plugin_id = $plugin_id;
        $this->plugin_sid = $plugin_sid;
        $this->reference_id = $reference_id;
        $this->reference_sid = $reference_sid;
    }
    function get_plugin_id() {
        return $this->plugin_id;
    }
    function get_plugin_sid() {
        return $this->plugin_sid;
    }
    function get_reference_id() {
        return $this->reference_id;
    }
    function get_reference_sid() {
        return $this->reference_sid;
    }
    function get_count($conn) {
        $sql = "SELECT count(*) FROM plugin_reference";
        if (!$rs = & $conn->Execute($sql)) {
            print $conn->ErrorMsg();
        } else {
            return $rs->fields["count(*)"];
        }
    }
    function get_list($conn, $args = "", $inf = 0, $sup = 25) {
        $i = 0;
        $query = OssimQuery("SELECT * FROM plugin_reference $args");
        if (!$rs = & $conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                if (($i++ >= $inf) && ($inf++ <= $sup)) {
                    $list[] = new Plugin_reference($rs->fields["plugin_id"], $rs->fields["plugin_sid"], $rs->fields["reference_id"], $rs->fields["reference_sid"]);
                } elseif ($inf > $sup) {
                    break;
                }
                $rs->MoveNext();
            }
        }
        return $list;
    }
}
?>
