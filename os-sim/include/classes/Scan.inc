<?php

define ('NMAP_PING_SCAN_COMMAND',   
    "%s -sP -n %s");

define ('NMAP_ROOT_SCAN_COMMAND',
    "%s -sS -O -sV -n %s");

define ('NMAP_PING_SCAN_REGEXP', 
    "/^Host (\d+\.\d+\.\d+\.\d+) appears to be up/i");

define ('NMAP_MAC_ADDRESS_REGEXP',
    "/^MAC Address: (\w{2}:\w{2}:\w{2}:\w{2}:\w{2}:\w{2}) (\(.*\))?$/i");

define ('NMAP_OS_REGEXP', 
    "/^Running: (.*)$/i");

define ('NMAP_SERVICE_REGEXP',
    "/^(\d+)\/(\w+)\s+(open|filtered|unfiltered)\s+(\w+)\s+(.*)$/i");


require_once ('ossim_conf.inc');
require_once ('classes/Log_action.inc');

class Scan {

    var $network;
    var $scan;
    var $nmap;

    function Scan ($network, $scan = NULL)
    {
        /* network to scan */
        $this->network = $network;

        /* store scan result */
        if (is_array($scan))
            $this->scan = $scan;
        else
            $this->scan = array ();

        /* nmap */
        $conf = $GLOBALS["CONF"];
        $this->nmap = $conf->get_conf("nmap_path");

        /* User action logging */
        $infolog = array($network);
        Log_action::log(43,$infolog);
   }

    function do_scan ()
    {
        $this->ping_scan();

        foreach ($this->scan as $host) {
            $ip = $host["ip"];
            
            echo " * $ip.. <br/>";
            flush();

            $this->root_scan($ip);
        }
    }

    function ping_scan()
    {
        $command = sprintf(NMAP_PING_SCAN_COMMAND, $this->nmap, $this->network);
        $this->__ping_scan_process_output
            ($this->__get_command_output($command));
    }

    function __ping_scan_process_output($output)
    {
        foreach ($output as $line) 
        {
            if (preg_match(NMAP_PING_SCAN_REGEXP, $line, $regs)) 
            {
                $this->scan[$regs[1]] = array ("ip"        => $regs[1],
                                               "mac"       => "",
                                               "os"        => "",
                                               "services"  => array ());
            }
        }
    }


    function root_scan($ip)
    {
        $command = sprintf(NMAP_ROOT_SCAN_COMMAND, $this->nmap, $ip);
        $this->__root_scan_process_output
            ($this->__get_command_output($command), $ip);
    }

    function __root_scan_process_output($output, $ip)
    {
        foreach ($output as $line)
        {
            /* mac */
            if (preg_match(NMAP_MAC_ADDRESS_REGEXP, $line, $regs)) {
                $this->scan[$ip]["mac"] = $regs[1];
                $this->scan[$ip]["mac_vendor"] = $regs[2];
            
            /* os */
            } elseif (preg_match(NMAP_OS_REGEXP, $line, $regs)) {
                $this->scan[$ip]["os"] = $regs[1];
            
            /* services */
            } elseif (preg_match(NMAP_SERVICE_REGEXP, $line, $regs)) {
                $this->scan[$ip]["services"][$regs[1]."/".$regs[2]] = array (
                    "port"      => $regs[1],
                    "proto"     => $regs[2],
                    "state"     => $regs[3],
                    "service"   => $regs[4],
                    "version"   => $regs[5]);
            }
        }
    }

    function __get_command_output($cmd)
    {
        $output = shell_exec(escapeshellcmd($cmd));
        return split("\n", $output);
    }

    function debug()
    {
        // Show scan result
        print "<pre>";
        print_r($this->scan);
        print "</pre>";
    }

    function save_scan()
    {
        $_SESSION["_scan"] = $this->scan;
    }

    function get_scan()
    {
        if (isset($_SESSION["_scan"])) {
            return $_SESSION["_scan"];
        }
    }

}

?>
