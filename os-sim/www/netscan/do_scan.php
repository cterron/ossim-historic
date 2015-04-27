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

require_once 'av_init.php';

Session::logcheck('environment-menu', 'ToolsScan');

ini_set('max_execution_time','1200');


$data['status']  = 'success';
$data['data']    = NULL;


$assets          = POST('assets');
$scan_mode       = POST('scan_mode');
$timing_template = POST('timing_template');
$custom_ports    = POST('custom_ports');
$sensor          = POST('sensor');
$only_stop       = intval(POST('only_stop'));
$autodetect      = (POST('autodetect') == '1') ? 1 : 0;
$rdns            = (POST('rdns') == '1') ? 1 : 0;
$custom_ports    = str_replace(' ', '', $custom_ports);

ossim_valid($scan_mode,       OSS_ALPHA, OSS_SCORE, OSS_NULLABLE,                 'illegal:' . _('Full scan'));
ossim_valid($timing_template, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE,                  'illegal:' . _('Timing_template'));
ossim_valid($custom_ports,    OSS_DIGIT, OSS_SPACE, OSS_SCORE, OSS_NULLABLE, ',', 'illegal:' . _('Custom Ports'));
ossim_valid($sensor,          OSS_HEX, OSS_ALPHA, OSS_NULLABLE,                   'illegal:' . _('Sensor'));
ossim_valid($only_stop,       OSS_DIGIT, OSS_NULLABLE,                            'illegal:' . _('Only stop'));


if (ossim_error())
{
    $data['status']  = 'error';
    $data['data']    = "<div style='text-align: left; padding: 0px 0px 3px 10px;'>"._('The following errors occurred').":</div>
                        <div class='error_item'>".ossim_get_error_clean()."</div>";

    echo json_encode($data);
    exit();
}


//Stop scan
if ($only_stop)
{
    $scan = new Scan();
    $scan->stop();

    $data['status'] = 'success';
    $data['data']   = NULL;

    echo json_encode($data);
    exit();
}


$scan_path_log = "/tmp/nmap_scanning_".md5(Session::get_secure_id()).'.log';

$assets_string = array();

if (is_array($assets) && count($assets) > 0)
{
    foreach ($assets as $asset)
    {
        //Only IP/CIDR is validated
        $_asset = explode('#', $asset);
        $_asset = (count($_asset) == 1) ? $_asset[0] : $_asset[1];

        ossim_valid($_asset, OSS_IP_ADDRCIDR, 'illegal:' . _('Asset'));

        if (ossim_error())
        {
            $data['status']  = 'error';
            $data['data']    = "<div style='text-align: left; padding: 0px 0px 3px 10px;'>"._('The following errors occurred').":</div>
                        <div class='error_item'>".ossim_get_error_clean()."</div>";

            echo json_encode($data);
            exit();
        }
        else
        {
            //IP_CIDR and ID is pushed
            array_push($assets_string, $asset);
        }
    }
}


$assets_p = implode(' ', $assets_string);

if($sensor == 'auto' || valid_hex32($sensor))
{
    //We use a remote sensor to perform the scan or Frameworkd machine if the local sensor is selected
    $rscan = new Remote_scan($assets_p, 'normal', $sensor);

    $last_error = $rscan->get_last_error();

    if (!empty($last_error['data']))
    {
        $data['status'] = $last_error['severity'];
        $data['data']   = $last_error['data'];
    }
    else
    {
        //Scanning sensor
        $scanning_sensor = $rscan->get_scanning_sensor();

        // Getting local sensor ID
        $db   = new Ossim_db();
        $conn = $db->connect();

        $admin_ip = Util::get_default_admin_ip();
        $res      = Av_center::get_system_info_by_ip($conn, $admin_ip);

        if ($res['status'] == 'success')
        {
            $local_sensor_id = $res['data']['sensor_id'];
        }

        $db->close();

        if ($scanning_sensor == $local_sensor_id)
        {
            $scanning_sensor = NULL;
        }
    }
}
else
{
    //We use Frameworkd machine to perform the scan
    $scanning_sensor = NULL;
}


if ($data['status'] == 'success')
{
    //Delete previous scan
    $scan = new Scan();
    $scan->delete_data();

    // Launch scan in background
    $cmd = "/usr/bin/php /usr/share/ossim/scripts/vulnmeter/remote_nmap.php '$assets_p' '$scanning_sensor' '$timing_template' '$scan_mode' '" . Session::get_session_user() . "' '$autodetect' '$rdns' '$custom_ports' > $scan_path_log 2>&1 &";

    system($cmd);
}

session_write_close();

echo json_encode($data);
