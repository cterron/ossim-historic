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
* - Insert()
* - Delete()
* - str2date()
* - array2str()
* - running_restoredb()
* Classes list:
* - Backup
*/
require_once ('classes/Session.inc');
require_once 'classes/Util.inc';
require_once 'ossim_db.inc';
require_once ('classes/Log_action.inc');
class Backup {
    function Insert($insert) {
        $conf = $GLOBALS["CONF"];
        $data_dir = $conf->get_conf("data_dir");
        $backup_dir = $conf->get_conf("backup_dir");
        $files = Backup::array2str($insert);
        if ($files != "") {
            $user = Session::get_session_user();
            $files = escapeshellcmd($files);
            $user = escapeshellcmd($user);
            //$res = popen("$data_dir/scripts/restoredb.pl insert $files $user &", "r");
            echo "Launching: $data_dir/scripts/restoredb.pl insert $files $user\n";
            shell_exec("nohup $data_dir/scripts/restoredb.pl insert $files $user >/tmp/restore_insert.log 2>/dev/null &");
            $isDisabled = 1;
        }
        $infolog = array(
            $files
        );
        Log_action::log(44, $infolog);
    }
    function Delete($delete) {
        $conf = $GLOBALS["CONF"];
        $data_dir = $conf->get_conf("data_dir");
        $backup_dir = $conf->get_conf("backup_dir");
        $files = Backup::array2str($delete);
        if ($files != "") {
            $user = Session::get_session_user();
            $files = escapeshellcmd($files);
            $user = escapeshellcmd($user);
            //$res = popen("$data_dir/scripts/restoredb.pl delete $files $user &", "r");
            echo "Launching: $data_dir/scripts/restoredb.pl delete $files $user\n";
            shell_exec("nohup $data_dir/scripts/restoredb.pl delete $files $user >/tmp/restore_delete.log 2>/dev/null &");
            $isDisabled = 1;
            //sleep(3);
            
        }
        $infolog = array(
            $files
        );
        Log_action::log(45, $infolog);
    }
    function str2date($str) {
        list($year, $month, $day) = sscanf($str, "%4s%2s%2s");
        return "$year-$month-$day";
    }
    function array2str($arr) {
        $str = "";
        if (is_array($arr)) {
            while (list($key, $value) = each($arr)) {
                if ($str == "") $str = $value;
                else $str.= "," . $value;
            }
        }
        return $str;
    }
    function running_restoredb() {
        if (file_exists("/tmp/ossim-restoredb.pid")) {
            return 1;
        } else {
            return 0;
        }
    }
} //class backup

?>

