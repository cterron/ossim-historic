<?php
/**
*
* License:
*
* Copyright (c) 2003-2006 ossim.net
* Copyright (c) 2007-2013 AlienVault
* All rights reserved.
*
* This package is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; version 2 dated June, 1991.
* You may not use, modify or distribute this program under any other version
* of the GNU General Public License.
*
* This package is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this package; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
* MA  02110-1301  USA
*
*
* On Debian GNU/Linux systems, the complete text of the GNU General
* Public License can be found in `/usr/share/common-licenses/GPL-2'.
*
* Otherwise you can read it here: http://www.gnu.org/licenses/gpl-2.0.txt
*
*/


ob_implicit_flush();
ini_set('include_path', '/usr/share/ossim/include');

require_once 'av_init.php';


$scan_modes = array(
    'ping'   => _('Ping'),
    'normal' => _('Normal'),
    'fast'   => _('Fast Scan'),
    'full'   => _('Full Scan'),
    'custom' => _('Custom')
);

$error_message   = array();

$targets         = $argv[1];
$remote_sensor   = $argv[2];
$timing_template = ($argv[3] != '' && $argv[3] != 'vulnscan') ? $argv[3] : '-T4';

// Special case
$argv[4] = ($argv[4] == 'root') ? 'full' : $argv[4];

if (array_key_exists($argv[4], $scan_modes))
{
    $scan_type = $argv[4];
}
else
{
    $scan_type = 'normal';
}


$user = $argv[5];

$autodetect = ($argv[6] == '0' || $argv[3] == 'vulnscan') ? FALSE : TRUE;
$rdns       = ($argv[7] == '0') ? FALSE : TRUE;
$ports      = $argv[8]; // When type is custom, specific ports


// Check targets
$target_array = explode(' ', $targets);

foreach ($target_array as $target)
{
    //Only IP/CIDR is validated
    $_target = explode('#', $target);
    $_target = (count($_target) == 1) ? $_target[0] : $_target[1];


    ossim_valid($_target, OSS_IP_ADDRCIDR, 'illegal:' . _('Target'));

    if (ossim_error())
    {
        $error_message[] = ossim_get_error_clean();
        ossim_set_error(FALSE);
    }
}

// Check remote sensor
if(!valid_hex32($remote_sensor) && $remote_sensor != 'null' && !empty($remote_sensor))
{
    ossim_valid($remote_sensor, OSS_IP_ADDR, 'illegal:' . _('Remote sensor'));
}

if (ossim_error())
{
    $error_message[] = ossim_get_error_clean();

    ossim_set_error(FALSE);
}

// check timing template
ossim_valid($timing_template, OSS_NULLABLE, OSS_TIMING_TEMPLATE, 'illegal:' . _('Timing Template'));
if (ossim_error())
{
    $error_message[] = ossim_get_error_clean();

    ossim_set_error(FALSE);
}

// check scan type
ossim_valid($scan_type, OSS_NULLABLE, OSS_ALPHA, 'illegal:' . _('Scan type'));
if (ossim_error())
{
    $error_message[] = ossim_get_error_clean();

    ossim_set_error(FALSE);
}

// check scan file
ossim_valid($user, OSS_NULLABLE, OSS_USER_2, 'illegal:' . _('User'));
if (ossim_error())
{
    $error_message[] = ossim_get_error_clean();

    ossim_set_error(FALSE);
}

// check ports
ossim_valid($ports, OSS_DIGIT, OSS_SPACE, OSS_SCORE, OSS_NULLABLE, ',', 'illegal:' . _('Custom Ports'));
if (ossim_error())
{
    $error_message[] = ossim_get_error_clean();
}

if (!empty($error_message))
{
    $status_message  = _('Scan could not be completed.  The following errors occurred').":\n".implode("\n", $error_message);

    die($status_message);
}


if ($remote_sensor != '' && $remote_sensor != 'null')
{
    $scan = new Remote_scan($targets, $scan_type, $remote_sensor, $user, $timing_template, $autodetect, $rdns, $ports);

    $quiet = ($timing_template != '') ? FALSE : TRUE;

    echo 'Scanning remote networks: '.$targets."\n";

    $scan->do_scan($quiet);

    $last_error = $scan->get_last_error();

    if (is_array($last_error) && !empty($last_error['data']))
    {
        $status_message = _('Scan could not be completed.  The following errors occurred').":\n".$last_error['data'];

        die($status_message);
    }
}
else
{
    echo 'Scanning local networks: '.$targets."\n";

    $only_ping = ($scan_type == 'ping' || $argv[3] == 'vulnscan') ? TRUE : FALSE;
    $config    = array('only_ping' => $only_ping, 'user' => $user);

    $scan = new Scan($targets, $config);

    if ($argv[3] != 'vulnscan')
    {
        // Append Timing
        $scan->append_option($timing_template);

        // Append Autodetect
        if ($autodetect)
        {
            if ($scan_type != 'fast')
            {
                $scan->append_option('-A');
            }
            else
            {
                $scan->append_option('-sV -O --osscan-guess --max-os-tries=1');
            }
        }
        // Append RDNS
        if (!$rdns)
        {
            $scan->append_option('-n');
        }

        if ($scan_type == 'fast')
        {
            $scan->append_option('-p21,22,23,25,53,80,113,115,135,139,161,389,443,445,554,1194,1241,1433,3000,3306,3389,8080,9390,27017');
        }
        elseif ($scan_type == 'custom')
        {
            $scan->append_option("-sS -p $ports");
        }
        elseif ($scan_type == 'normal')
        {
            $scan->append_option('-sS');
        }
        elseif ($scan_type == 'full')
        {
            $scan->append_option('-sS -p 1-65535');
        }
    }

    // ping scan
    $scan->search_hosts();

    $status = $scan->get_status();

    while($status == 'Searching Hosts')
    {
        $status = $scan->get_status();
        sleep(2);
    }

    // Normal scan
    if ($scan_type != 'ping' && $argv[3] != 'vulnscan')
    {
        $scan->launch_scan();

        while($scan->get_status() == 'Scanning Hosts')
        {
            $progress = $scan->get_progress();
            echo $scan->get_status() . ': ' . $progress['hosts_scanned']. '/'.$progress['total_hosts'].'  '.$progress['remaining']. "\n";
            sleep(2);
        }
    }
}


if ($argv[3] == 'vulnscan')
{
    $ips = array();

    if (is_object($scan))
    {
        // Getting discovered hosts
        $ips = (get_class($scan) == 'Remote_scan') ? $scan->get_scan() : $scan->get_results();

        // Delete results
        $scan->delete_data();
    }

    if (is_array($ips['scanned_ips']) && !empty($ips['scanned_ips']))
    {
        foreach ($ips['scanned_ips'] as $ip => $val)
        {
            echo "Host $ip appears to be up\n";
        }
    }
}
