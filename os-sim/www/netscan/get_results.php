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

require_once 'av_init.php';
require_once 'scan_util.php';

Session::logcheck('environment-menu', 'ToolsScan');


$scan_path_log = "/tmp/nmap_scanning_".md5(Session::get_secure_id()).'.log';


$data['status'] = 'success';
$data['data']   = NULL;

if (file_exists($scan_path_log))
{
    $log_file = file_get_contents($scan_path_log);

    if (preg_match('/Scan could not be completed.(.*)/s', $log_file, $matches))
    {
        $data['status'] = 'error';
        $data['data']   = nl2br($matches[0]);

        @unlink($scan_path_log);

        echo json_encode($data);
        exit();
    }

    @unlink($scan_path_log);
}


$db   = new ossim_db();
$conn = $db->connect();

$scan     = new Scan();
$lastscan = $scan->get_results();



if (!empty($lastscan['scanned_ips']))
{
    ob_start();

    scan2html($conn, $lastscan);

    $data['data'] = ob_get_contents();

    ob_end_clean();
}
else
{
    $data['status'] = 'warning';
    $data['data']   = _("The scan has been completed. We couldn't find any host within the selected networks");

    $scan->delete_data();
}

$db->close();

echo json_encode($data);
