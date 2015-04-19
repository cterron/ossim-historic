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
 * - Scan()
 * - do_scan()
 * - ping_scan()
 * - __ping_scan_process_output()
 * - root_scan()
 * - __root_scan_process_output()
 * - __get_command_output()
 * - debug()
 * - save_scan()
 * - get_scan()
 * - del_scan()
 * Classes list:
 * - Scan
 */
define('NMAP_PING_SCAN_COMMAND', "%s -sP -n %s");
define('NMAP_ROOT_SCAN_COMMAND', "%s -sS -O -sV -n %s");
define('NMAP_PING_SCAN_REGEXP', "/^Host (\d+\.\d+\.\d+\.\d+) appears to be up/i");
define('NMAP_MAC_ADDRESS_REGEXP', "/^MAC Address: (\w{2}:\w{2}:\w{2}:\w{2}:\w{2}:\w{2}) (\(.*\))?$/i");
define('NMAP_OS_REGEXP', "/^Running: (.*)$/i");
define('NMAP_SERVICE_REGEXP', "/^(\d+)\/(\w+)\s+(open|filtered|unfiltered)\s+(\w+)\s+(.*)$/i");
require_once ('ossim_conf.inc');
require_once ('classes/Log_action.inc');
class Scan {
    var $network;
    var $scan;
    var $nmap;
    function Scan($network, $scan = NULL) {
        /* network to scan */
        $this->network = $network;
        /* store scan result */
        if (is_array($scan)) $this->scan = $scan;
        else $this->scan = array();
        /* nmap */
        $conf = $GLOBALS["CONF"];
        $this->nmap = $conf->get_conf("nmap_path");
        /* User action logging */
        $infolog = array(
            $network
        );
        Log_action::log(43, $infolog);
    }
    function do_scan($full_scan = TRUE) {
        $this->ping_scan();
        foreach($this->scan as $host) {
            $ip = $host["ip"];
            echo " * $ip.. <br/>";
            flush();
            if ($full_scan) {
                $this->root_scan($ip);
            }
        }
    }
    function ping_scan() {
        $command = sprintf(NMAP_PING_SCAN_COMMAND, $this->nmap, $this->network);
        $this->__ping_scan_process_output($this->__get_command_output($command));
    }
    function __ping_scan_process_output($output) {
        foreach($output as $line) {
            if (preg_match(NMAP_PING_SCAN_REGEXP, $line, $regs)) {
                $this->scan[$regs[1]] = array(
                    "ip" => $regs[1],
                    "mac" => "",
                    "mac_vendor" => "",
                    "os" => "",
                    "services" => array()
                );
            }
        }
    }
    function root_scan($ip) {
        $command = sprintf(NMAP_ROOT_SCAN_COMMAND, $this->nmap, $ip);
        $this->__root_scan_process_output($this->__get_command_output($command) , $ip);
    }
    function __root_scan_process_output($output, $ip) {
        foreach($output as $line) {
            /* mac */
            if (preg_match(NMAP_MAC_ADDRESS_REGEXP, $line, $regs)) {
                $this->scan[$ip]["mac"] = strtoupper($regs[1]);
                $this->scan[$ip]["mac_vendor"] = $regs[2];
                /* os */
            } elseif (preg_match(NMAP_OS_REGEXP, $line, $regs)) {
                $this->scan[$ip]["os"] = $regs[1];
                /* services */
            } elseif (preg_match(NMAP_SERVICE_REGEXP, $line, $regs)) {
                $this->scan[$ip]["services"][$regs[1] . "/" . $regs[2]] = array(
                    "port" => $regs[1],
                    "proto" => $regs[2],
                    "state" => $regs[3],
                    "service" => $regs[4],
                    "version" => $regs[5]
                );
            }
        }
    }
    function __get_command_output($cmd) {
        $output = shell_exec(escapeshellcmd($cmd));
        return split("\n", $output);
    }
    function debug() {
        // Show scan result
        print "<pre>";
        print_r($this->scan);
        print "</pre>";
    }
    function save_scan() {
        $_SESSION["_scan"] = $this->scan;
    }
    function get_scan() {
        if (isset($_SESSION["_scan"])) {
            return $_SESSION["_scan"];
        }
    }
    function del_scan() {
        if (isset($_SESSION["_scan"])) {
            unset($_SESSION["_scan"]);
        }
    }
}
?>
