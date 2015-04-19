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
* - getCategoryName()
* - save()
* - showSubCategoryHTML()
* - showSettingsHTML()
* - showWindowContents()
* Classes list:
* - Plugin_Maps extends Panel
*/
require_once 'classes/Session.inc';
require_once 'classes/User_config.inc';
require_once 'ossim_db.inc';
class Plugin_Maps extends Panel {
    var $defaults = array(
        'map_id' => 0,
        'lon' => 0,
        'lat' => 0,
        'zoom' => 0,
        'controls' => 0,
        'max-zoom' => 0,
        'min-zoom' => 15
    );
    function getCategoryName() {
        return _("Maps");
    }
    function save() {
        $db = new ossim_db();
        $conn = $db->connect();
        $config = new User_config($conn);
        $login = Session::get_session_user();
        $opts = $this->get();
        $window_id = $opts['window_opts']['id'];
        // Ej: store in var '1x1' the plugin options using the category 'panel'
        $config->set($login, $window_id, $opts['plugin_opts'], 'php', 'panel');
        $db->close($conn);
        return $opts;
    }
    function showSubCategoryHTML() {
        $html = _("Choose map") . ':<br/>';
        $map_id = $this->get('map_id');
        $db = new ossim_db();
        $conn = $db->connect();
        $sql = "SELECT id, name, engine FROM map";
        $rows = $conn->GetArray($sql);
        if ($rows === false) {
            return $conn->ErrorMsg();
        } elseif (!count($rows)) {
            return _("No maps found");
        }
        foreach($rows as $row) {
            $checked = $map_id == $row['id'] ? 'checked=on' : '';
            $html.= "<input type='radio' name='map_id' value='{$row['id']}' $checked>{$row['name']} - {$row['engine']}<br/>";
        }
        return $html;
    }
    function showSettingsHTML() {
        return _("No extra settings needed");
        /*
        $html = '';
        $html .= "&nbsp;"._("Lat").": <input name='lat' size='18' value='".$this->get("lat")."'><br/>";
        $html .= _("Lon").": <input name='lon' size='18' value='".$this->get("lon")."'><br/>";
        $html .= _("Zoom").": <select name='zoom'>";
        foreach (range(0, 15) as $z) {
        $check = $this->get('zoom') == $z ? "selected" : '';
        $html .= "<option value='$z' $check>$z</option>\n";
        }
        $html .= "</select><br/>";
        $check_yes = $check_no = '';
        if ($this->get('controls') == 1) {
        $check_yes = 'checked';
        } else {
        $check_no = 'checked';
        }
        $html .= _("Show controls").": ".
        "<input type='radio' name='controls' value='0' $check_no> "._("no")."&nbsp;".
        "<input type='radio' name='controls' value='1' $check_yes> "._("yes")."<br/>";
        
        $html .= _("Max zoom").": <input type='text' name='max-zoom' value='".$this->get("max-zoom")."' size=2>&nbsp;".
        _("Min zoom").": <input type='text' name='min-zoom' value='".$this->get("min-zoom")."' size=2><br/>";
        
        $html .= "<br/><i>"._("Set 'Show controls' to yes and press 'Update Output' for zoom and lat/lon information")."</i>";
        return $html;
        */
    }
    function showWindowContents() {
        //return printr($this->get(), false, true);
        //$window_id = $this->get('id', 'window_opts');
        if (!$this->get('map_id')) {
            return _("Please select a map in the window configuration");
        }
        return '<iframe id="maps-iframe" name="maps-iframe" width="500" height="500" scrolling="no" frameborder="0"
                 src="../maps/draw_openlayers.php?panel_id=' . GET('panel_id') . '&map_id=' . $this->get('map_id') . '">IFRAMES not supported by your browser</iframe>';
    }
}
?>
