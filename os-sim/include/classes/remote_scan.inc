<?php
/**
* remote_scan.inc
*
* File remote_scan.inc is used to:
*   - To manage remote Nmap scans
*
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
* @package    ossim-framework\Assets
* @autor      AlienVault INC
* @license    http://www.gnu.org/licenses/gpl-2.0.txt
* @copyright  2003-2006 ossim.net
* @copyright  2007-2013 AlienVault
* @link       https://www.alienvault.com/
*/


/**
* Remote_scan Class
*
* Class to manage remote Nmap scans
*
* @package    ossim-framework\Asset
* @autor      AlienVault INC
* @copyright  2007-2013 AlienVault
* @link       https://www.alienvault.com/ AlienVault web page
*/

class Remote_scan
{
    /**
    * Networks and hosts to be scanned
    *
    * @var array
    * @access private
    */
    private $targets;

    /**
    * Array to resolve IPs and IDs
    *
    * @var array
    * @access private
    */
    private $sensor_cache;

    /**
    * Nmap scan type
    *
    * @var string
    * @access private
    */
    private $type;

    /**
    * Scanning sensor (IP address or Host ID)
    *
    * @var string
    * @access private
    */
    private $scanning_sensor;

    /**
    * Error information
    *
    * @var array
    * @access private
    */
    private $error;

    /**
    * Frameworkd configuration (IP address, port and timeout)
    *
    * @var array
    * @access private
    */
    private $f_config;

    /**
    * Scan results
    *
    * @var string
    * @access private
    */
    private $scan;

    /**
    * Connected agents (Sensors)
    *
    * @var array
    * @access private
    */
    private $c_sensors;

    /**
    * Timing template option
    *
    * @var string
    * @access private
    */
    private $timing_template;

    /**
    * OS detection and version detection option
    *
    * @var string
    * @access private
    */
    private $autodetect;

    /**
    * Reverse DNS option
    *
    * @var string
    * @access private
    */
    private $rdns;

    /**
    * Ports to scan
    *
    * @var string
    * @access private
    */
    private $ports;

    /**
    * Prefix to select last report
    *
    * @var string
    * @access private
    */
    private $prefix;

    /**
    * Nmap parser object
    *
    * @var object
    * @access private
    */
    private $nmap_parser;

     /**
    * www-data to change files with chown
    *
    * @var string
    * @access private
    */
    private $owner;
    
    /**
    * Class constructor
    *
    * This function sets up the class
    *
    * @param string  $targets          [Optional] Networks and hosts to be scanned
    * @param string  $type             [Optional] Scan type
    * @param string  $scanning_sensor  [Optional] Sensor ID or Sensor IP
    * @param string  $user             [Optional] User that will launch the scan
    * @param string  $scan_path        [Optional] File with Nmap results
    * @param string  $timing_template  [Optional] Timing template option
    * @param string  $autodetect       [Optional] OS detection and version detection option
    * @param string  $rdns             [Optional] Reverse DNS option
    * @param string  $ports            [Optional] Ports to scan
    */
    public function __construct($targets = '', $type = 'normal', $scanning_sensor = '', $user = '', $timing_template = 'T3', $autodetect = 'enable', $rdns = '1', $ports = '')
    {
        //Initialize error by default
        $this->error = array (
            'data'     => '',
            'severity' => ''
        );

        //Frameworkd configuration
        $this->set_frameworkd_configuration();


        // Scan configuration
        $this->owner           = 'www-data';
        $this->type            = $type;
        $this->autodetect      = $autodetect;
        $this->rdns            = $rdns;
        $this->ports           = $ports;
        $this->timing_template = str_replace('-',' ', $timing_template);
        $this->prefix          = md5(time());


        //Normalize targets
        $this->targets = array(
            'nmap_format' => '',
            'av_format'   => $av_format
        );

        if (!empty($targets))
        {
            $assets      = explode(' ', $targets);
            $nmap_format = array();
            $av_format   = array();

            foreach($assets as $asset)
            {
                if (preg_match('/(.*)#((\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})(\/\d{1,2})?)/', $asset, $fnd))
                {
                    $type       = ($fnd[4] == '/32') ? 'host' : 'net';
                    $id         = trim($fnd[1]);
                    $ip_cidr    = ($type == 'host') ? trim($fnd[3]) : trim($fnd[2]);
                    $namp_asset = trim($fnd[2]);
                }
                elseif(preg_match('/^(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})(\/\d{1,2})?$/', $asset, $fnd))
                {
                    $type       = ($fnd[2] == '/32') ? 'host' : 'net';
                    $id         = NULL;
                    $ip_cidr    = ($type == 'host') ? trim($fnd[1]) : trim($fnd[0]);
                    $namp_asset = trim($fnd[0]);
                }

                if (!empty($ip_cidr))
                {
                    $av_format[$asset] = array(
                        'type'    => $type,
                        'ip_cidr' => $ip_cidr,
                        'id'      => $id
                    );

                    $nmap_format[$namp_asset] = $namp_asset;
                }
            }

            $this->targets = array(
                'nmap_format' => implode(' ', $nmap_format),
                'av_format'   => $av_format
            );
        }


        //Creating sensor hash to resolve IPs and IDs
        $db   = new Ossim_db();
        $conn = $db->connect();

        $filters     = array('order_by' => 'priority desc');
        $all_sensors = Av_sensor::get_list($conn, $filters);

        foreach ($all_sensors[0] as $sensor_id => $sensor_data)
        {
            $this->sensor_cache[$sensor_data['ip']] = $sensor_id;
            $this->sensor_cache[$sensor_id]         = $sensor_data['ip'];
        }

        //Calculating connected agents
        $this->get_connected_agents();

        //Setting scanning sensor
        $this->set_scanning_sensor($conn, $scanning_sensor);

        $db->close();

        //Initialize NMAP_parser Object
        $this->nmap_parser = new Nmap_parser();

        $this->set_path_xml_file($user);
    }


    /**
    * This function sets Frameworkd configuration
    *
    * @return void
    */
    private function set_frameworkd_configuration()
    {
        if (!$GLOBALS['CONF'])
        {
            $GLOBALS['CONF'] = new Ossim_conf();
        }

        $ossim_conf = $GLOBALS['CONF'];

        // Set Frameworkd configuration
        $this->f_config = array(
            'address' => '127.0.0.1',
            'port'    => $ossim_conf->get_conf('frameworkd_port'),
            'timeout' => array('sec' => 5, 'usec' => 0)
        );
    }

    /**
    * This function sets path from XML file which saves the scan
    *
    * @param string  $user  [Optional] User that will launch the scan
    *
    * @return void
    */
    private function set_path_xml_file($user = '')
    {
        $user_id = ($user != '') ? md5(Session::get_secure_id($user)) : Util::uuid();

        $this->xml_output = "/var/tmp/nmap_scan_$user_id.xml";
    }



    /**
    * This function gets the scanning sensor
    *
    * @return string
    */
    public function get_scanning_sensor()
    {
        return $this->scanning_sensor;
    }


    /**
    * This function sets the scanning sensor
    *
    * @param object $conn             Database access object
    * @param string $scanning_sensor  [Optional] Sensor (NULL, Sensor ID or Sensor IP, 'local' or 'auto')
    *
    * @throws Av_exception If a connection error occurred
    *
    * @return boolean
    */
    public function set_scanning_sensor($conn, $scanning_sensor = '')
    {
        Ossim_db::check_connection($conn);

        //Seting default scanning sensor
        $this->scanning_sensor = NULL;


        // No agents connected (It will use the Frameworkd)
        if (count($this->c_sensors) == 0)
        {
            $error_msg = _('No sensors available');

            $this->set_error('warning', $error_msg);

            return FALSE;
        }


        // Getting local sensor ID
        $admin_ip = Util::get_default_admin_ip();
        $res      = Av_center::get_system_info_by_ip($conn, $admin_ip);

        if ($res['status'] == 'success')
        {
            $local_sensor_id = $res['data']['sensor_id'];
        }


        if (empty($scanning_sensor) || $scanning_sensor == 'local')
        {
            // Local sensor
            if ($local_sensor_id != '' && array_key_exists($local_sensor_id, $this->c_sensors))
            {
                $this->scanning_sensor = $local_sensor_id;

                return TRUE;
            }
            else
            {
                $error_msg = _('Warning! Local sensor is not available at this time.  Please, choose another sensor');
                $this->set_error('warning', $error_msg);

                return FALSE;
            }
        }
        elseif(valid_hex32($scanning_sensor) || Asset_host_ips::valid_ip($scanning_sensor))
        {
            //An specific sensor was selected

            $scanning_sensor = (Asset_host_ips::valid_ip($scanning_sensor)) ? $this->sensor_cache[$scanning_sensor] : $scanning_sensor;

            if (array_key_exists($scanning_sensor, $this->c_sensors))
            {
                $this->scanning_sensor = $scanning_sensor;

                return TRUE;
            }
            else
            {
                $error_msg = _('Warning! Selected sensor is not available at this time.  Please, choose another sensor');

                $this->set_error('warning', $error_msg);

                return FALSE;
            }
        }
        elseif ($scanning_sensor == 'auto')
        {
            //Automatic mode (Asset permissions are used to calculate automatically the scanning sensor)

            $related_sensors = $this->c_sensors;

            foreach($this->targets['av_format'] as $asset)
            {
                if ($asset['type'] == 'net')
                {
                    if (!empty($asset['id']))
                    {
                        //Net with ID
                        $rs_by_asset     = Asset_net_sensors::get_sensors_by_id($conn, $asset['id']);
                        $related_sensors = array_intersect_key($related_sensors, $rs_by_asset);
                    }
                    else
                    {
                        //Net without ID

                        //Getting closest inventoried net
                        $closest_nets = Asset_net::get_closest_nets($conn, $asset['ip_cidr']);

                        if (is_array($closest_nets) && !empty($closest_nets))
                        {
                            $cn_key      = key($closest_nets);
                            $net_id      = $closest_nets[$cn_key]['id'];

                            $rs_by_asset = Asset_net_sensors::get_sensors_by_id($conn, $net_id);
                        }
                        else
                        {
                            $rs_by_asset = array();
                        }

                        $related_sensors = array_intersect_key($related_sensors, $rs_by_asset);
                    }
                }
                elseif ($asset['type'] == 'host')
                {
                    if (!empty($asset['id']))
                    {
                        //Host with ID
                        $rs_by_asset     = Asset_host_sensors::get_sensors_by_id($conn, $asset['id']);
                        $related_sensors = array_intersect_key($related_sensors, $rs_by_asset);
                    }
                    else
                    {
                        //Host without ID
                        $rs_by_asset     = Asset_host_sensors::get_sensors_by_ip($conn, $asset['ip_cidr']);
                        $related_sensors = array_intersect_key($related_sensors, $rs_by_asset);
                    }
                }
                else
                {
                    $related_sensors = array();
                }
            }

            // No sensor found  to scan all assets
            if (count($related_sensors) < 1)
            {
                $error_msg = _("Unable to select the scanning sensor using automatic mode.<br/><br/>
                It could be because some assets are not associated with a valid scanning sensor or there is no common sensor to scan all assets.<br/>
                Please, remove these assets from the select box or choose an specific sensor.");

                $this->set_error('warning', $error_msg);

                return FALSE;
            }
            else
            {
                if (array_key_exists($local_sensor_id, $related_sensors))
                {
                    // If local sensor is available, we use it
                    $this->scanning_sensor = $local_sensor_id;
                }
                else
                {
                    // Otherwise, we use the fist available sensor
                    $this->scanning_sensor = array_shift(array_keys($related_sensors));
                }

                return TRUE;
            }
        }
    }


    /**
    * This function gets connected agents (sensors)
    *
    * @return boolean
    */
    private function get_connected_agents()
    {
        $max_agents = 50;

        //Try to connect with frameworkd and "getconnectedagents"
        $socket = socket_create(AF_INET, SOCK_STREAM, 0);
        if ($socket < 0)
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $error_msg = _('Warning! Unable to create a connection with frameworkd. Reason:').' '.$last_socket_error;

            $this->set_error('warning', $error_msg);

            socket_close($socket);

            return FALSE;
        }


        // Connecting ...
        socket_set_block($socket);
        socket_set_option($socket, SOL_SOCKET, SO_RCVTIMEO, $this->f_config['timeout']);
        socket_set_option($socket, SOL_SOCKET, SO_SNDTIMEO, $this->f_config['timeout']);

        $result = @socket_connect($socket, $this->f_config['address'], $this->f_config['port']);

        if (!$result)
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $error_msg = _("Warning! Can't connect with frameworkd").' ('.$this->f_config['address'].':'.$this->f_config['port'].')';
            $error_msg .= ' '._('Reason').': '.$last_socket_error;

            $this->set_error('warning', $error_msg);

            socket_close($socket);

            return FALSE;
        }


        /* Getting total number of agents */
        $in = 'control action="gettotalagentsconnected"' . "\n";
        $out = '';

        socket_write($socket, $in, strlen($in));
        $out = @socket_read($socket, 5120, PHP_BINARY_READ);
        if (!preg_match("/ack(end)?$/", $out))
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $out = (empty($out)) ? _('empty response') : $out;

            $error_msg = _('Error! Bad response from frameworkd').": $out. "._('Reason:').' '.$last_socket_error;

            $this->set_error('error', $error_msg);

            socket_close($socket);

            return FALSE;
        }

        preg_match('/gettotalagentsconnected count\=\"(\d+)\"/', $out, $found);

        if (count($found) < 1)
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $out = (empty($out)) ? _('empty response') : $out;

            $error_msg = _('Error! Bad response from frameworkd').": $out. "._('Reason:').' '.$last_socket_error;

            $this->set_error('error', $error_msg);

            socket_close($socket);

            return FALSE;
        }

        if ($found[1] < 1)
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $error_msg = _('Error! No sensors connected').'. '._('Reason:').' '.$last_socket_error;

            $this->set_error('error', $error_msg);

            socket_close($socket);

            return FALSE;

        }

        $total = $found[1];
        $first = TRUE;


        // Getting agent information with pagination
        $s_agents = array();

        for($begin = 0; $begin < $total-1 || $first; $begin += $max_agents)
        {
            $in  = 'control action="getconnectedagents_p" begin_index="'.(($begin > 0) ? $begin + 1 : $begin).'" end_index="'.($begin + $max_agents).'"' . "\n";
            $out = '';

            socket_write($socket, $in, strlen($in));
            $out = @socket_read($socket, 5120, PHP_BINARY_READ);

            if (!preg_match("/ack(end)?$/", $out))
            {
                $last_socket_error = socket_strerror(socket_last_error($socket));

                $out = (empty($out)) ? _('empty response') : $out;

                $error_msg = _('Error! Bad response from frameworkd').": $out. "._('Reason:').' '.$last_socket_error;

                $this->set_error('error', $error_msg);

                socket_close($socket);

                return FALSE;
            }

            preg_match('/names=\"(.*?)\"/', $out, $found);

            if (count($found) < 2)
            {
                $last_socket_error = socket_strerror(socket_last_error($socket));

                $out = (empty($out)) ? _('empty response') : $out;

                $error_msg = _('Error! Bad response from frameworkd').": $out. "._('Reason:').' '.$last_socket_error;

                $this->set_error('error', $error_msg);

                socket_close($socket);

                return FALSE;
            }

            $found[1] = explode('|', $found[1]);

            foreach($found[1] as $a)
            {
                $data = explode('=', $a);

                // Pairs Sensor ID - Sensor name
                $s_id = $this->sensor_cache[$data[1]];
                if (!empty($s_id))
                {
                    $s_agents[$s_id] = $data[0];
                }
            }

            $first = FALSE;
        }

        socket_close($socket);

        // No sensor information
        if (count($s_agents) < 1)
        {
            $error_msg = _('Error! No sensor available');

            $this->set_error('error', $error_msg);

            return FALSE;
        }

        $this->c_sensors = $s_agents;

        return TRUE;
    }


    /**
    * This function returns all connected agents (sensors)
    *
    * @return array
    */
    public function get_sensors()
    {
        return $this->c_sensors;
    }


    /**
    * This function launches a scan and retrieves the results
    *
    * @param boolean $quiet  [Optional] Show results on screen
    * @param boolean $idm    [Optional] Only launch an scan with idm option and finish
    *
    * @return boolean
    */
    public function do_scan($quiet = TRUE, $idm = FALSE)
    {
        session_write_close();

        //No scan will be launched if there is an error
        $last_error = $this->get_last_error();

        if (is_array($last_error) && !empty($last_error['data']))
        {
            return FALSE;
        }

        $this->scan = '';

        // prepare to launch nmap
        $socket = socket_create(AF_INET, SOCK_STREAM, 0);

        if ($socket < 0)
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $error_msg = _('Warning! Unable to create a connection with frameworkd. Reason:').' '.$last_socket_error;

            $this->set_error('warning', $error_msg);

            socket_close($socket);

            return FALSE;
        }

        /* connect */
        socket_set_block($socket);
        socket_set_option($socket,SOL_SOCKET,SO_RCVTIMEO,$this->f_config['timeout']);
        socket_set_option($socket,SOL_SOCKET,SO_SNDTIMEO,$this->f_config['timeout']);
        $result = @socket_connect($socket, $this->f_config['address'], $this->f_config['port']);

        if (!$result)
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $error_msg = _("Warning! Can't connect with frameworkd").' ('.$this->f_config['address'].':'.$this->f_config['port'].')';
            $error_msg .= ' '._('Reason').': '.$last_socket_error;

            $this->set_error('warning', $error_msg);

            socket_close($socket);

            return FALSE;
        }


        // Execute NMAP
        // control action="nmap_scan" id="192.168.12.160" type="full" target="192.168.12.160/32" timming="T2" autodetect="enable"

        $scanning_sensor_ip = $this->sensor_cache[$this->scanning_sensor];

        $in = 'control action="nmap_scan" report_prefix="'.$this->prefix.'" id="'.$scanning_sensor_ip .'" type="'.$this->type.'" target="'.$this->targets['nmap_format'].'"';

        if ($this->timing_template != '')
        {
            $in .= ' timming="'.$this->timing_template.'"';
        }

        $in .= ($this->autodetect) ? ' autodetect="enabled"' : ' autodetect="disabled"';
        $in .= ($this->rdns) ? ' rdns="enable"' : ' rdns="disable"';

        if ($this->type == "custom" && $this->ports != '')
        {
            $in .= ' scan_ports="'.$this->ports.'"';
        }
        
        if ($idm)
        {
            $in .= ' idm="true"';
        }
        
        $in .= "\n";

        $out = '';
        socket_write($socket, $in, strlen($in));
        $out = @socket_read($socket, 2048, PHP_BINARY_READ);

        if (!preg_match("/ack(end)?$/",$out))
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $out = (empty($out)) ? _('empty response') : $out;

            $error_msg = _('Error! Bad response from frameworkd').": $out. "._('Reason:').' '.$last_socket_error;

            $this->set_error('error', $error_msg);

            socket_close($socket);

            return FALSE;
        }

        if ($idm) // Finish after launch it
        {
            socket_close($socket);

            return TRUE;
        }
        
        // Query until finish nmap job
        $in  = 'control action="nmap_status" id="'.$scanning_sensor_ip.'"' . "\n";
        $out = '';

        $status = 1;

        if (!$quiet)
        {
            echo _("Waiting remote scan status\n");
        }

        while ($status > 0)
        {
            if (!$quiet)
            {
                echo " .";
            }

            socket_write($socket, $in, strlen($in));
            $out = socket_read($socket, 2048, PHP_BINARY_READ);

            if (!preg_match('/ack(end)?$/',$out))
            {
                $last_socket_error = socket_strerror(socket_last_error($socket));

                $out = (empty($out)) ? _('empty response') : $out;

                $error_msg = _('Error! Bad response from frameworkd').": $out. "._('Reason:').' '.$last_socket_error;

                $this->set_error('error', $error_msg);

                socket_close($socket);

                return FALSE;
            }

            preg_match('/status=\"(.*?)\"/', $out, $found);

            $status=$found[1];

            sleep(10); // 10 seconds between requests
        }

        if (!$quiet)
        {
            echo '<br/><br/>';
        }

        // get scans with nmap_report_list
        $in  = 'control action="nmap_report_list" id="'.$scanning_sensor_ip .'"' . "\n";
        $out = '';
        socket_write($socket, $in, strlen($in));

        $exit = FALSE;
        while (!$exit && ($out = @socket_read($socket, 2048, PHP_BINARY_READ)) !== FALSE)
        {
            $report_list .= $out;
            if (preg_match('/ackend$/',rtrim($out)))
            {
                $exit = TRUE;
            }
        }

        if (!preg_match('/ack(end)?$/',$report_list))
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $error_msg = _('Error! Bad response from frameworkd').": $in >> $report_list. "._('Reason:').' '.$last_socket_error;

            $this->set_error('error', $error_msg);

            socket_close($socket);

            return FALSE;
        }

        // Select last report
        preg_match_all('/report=\"(.*?)\"/',$report_list,$found);

        foreach ($found[1] as $rpt)
        {
            if ( preg_match("/".$this->prefix."/", $rpt) )
            {
                $report = $rpt;
            }
        }

        //
        // Request report results
        //
        //echo 'control action="nmap_report_raw_get" id="'.$scanning_sensor_ip.'" path="'.$report.'"' . "\n"; // DEBUG report_prefix="'.$this->prefix.'"
        $in         = 'control action="nmap_report_raw_get"  id="'.$scanning_sensor_ip.'" path="'.$report.'"' . "\n";


        $out        = '';
        $xml_output = '';
        $output     = '';
        $exit       = FALSE;

        socket_write($socket, $in, strlen($in));

        while (!$exit && ($out = @socket_read($socket, 2048, PHP_BINARY_READ)) !== FALSE)
        {
            $output .= $out;
            if (preg_match('/ackend$/',rtrim($out)))
            {
                $exit = TRUE;
            }
        }

        //Delete remote scan
        $this->delete_scan($report);

        $out = trim($output);

        if (!preg_match('/ack(end)?$/', $out))
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $out = (empty($out)) ? _('empty response') : $out;

            $error_msg = _('Error! Bad response from frameworkd').": $out. "._('Reason:').' '.$last_socket_error;

            $this->set_error('error', $error_msg);

            socket_close($socket);

            return FALSE;
        }

        $lines = explode("\n", $output);

        foreach($lines as $line)
        {
            if(preg_match('/line=\"(.*)\"/',$line,$found))
            {
                $data     = $found[1];
                $data_bin = Util::hex2bin($data);

                $xml_output .= trim(gzuncompress($data_bin))."\n";
            }
        }

        /* Add sensor information to scan result (Necessary for setting correct CTX to insert new hosts)*/
        if (!empty($xml_output))
        {
            $xml = @simplexml_load_string($xml_output);
            $xml->addAttribute('av_sensor', $this->scanning_sensor);
            $xml_output = $xml->asXML();
        }

        $this->scan = $xml_output;

        //Show results
        if (!$quiet)
        {
            $this->print_scan();
            $this->save_scan();
        }

        socket_close($socket);

        return TRUE;
    }


    /**
    * This function returns the scan reports
    *
    * @return boolean | array
    */
    public function get_scans()
    {
        // prepare to launch nmap
        $socket = socket_create(AF_INET, SOCK_STREAM, 0);
        if ($socket < 0)
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $error_msg = _('Warning! Unable to create a connection with frameworkd. Reason:').' '.$last_socket_error;

            $this->set_error('warning', $error_msg);

            socket_close($socket);

            return FALSE;
        }

        /* connect */
        socket_set_block($socket);
        socket_set_option($socket,SOL_SOCKET,SO_RCVTIMEO,$this->f_config['timeout']);
        socket_set_option($socket,SOL_SOCKET,SO_SNDTIMEO,$this->f_config['timeout']);
        $result = @socket_connect($socket, $this->f_config['address'], $this->f_config['port']);

        if (!$result)
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $error_msg = _("Warning! Can't connect with frameworkd").' ('.$this->f_config['address'].':'.$this->f_config['port'].')';
            $error_msg .= ' '._('Reason').': '.$last_socket_error;

            $this->set_error('warning', $error_msg);

            socket_close($socket);

            return FALSE;
        }

        // Send nmap_report_list for each id */
        $reports = array();

        foreach($this->c_sensors as $cs_id => $cs_name)
        {
            if (trim($cs_name) != '')
            {
                //Get sensor IP
                $cs_ip = $this->sensor_cache[$idip];
                $in = 'control action="nmap_report_list" id="'.$cs_ip.'"' . "\n";

                $out    = '';
                $output = '';
                socket_write($socket, $in, strlen($in));

                $exit   = FALSE;

                while (!$exit && ($out = @socket_read($socket, 2048, PHP_BINARY_READ)) !== FALSE)
                {
                    $output .= $out;
                    if (preg_match('/ackend$/',rtrim($out)))
                    {
                        $exit = TRUE;
                    }
                }

                if (!preg_match('/ack(end)?$/', $output))
                {
                    $last_socket_error = socket_strerror(socket_last_error($socket));

                    $out = (empty($output)) ? _('empty response') : $output;

                    $error_msg = _('Error! Bad response from frameworkd').": $out. "._('Reason:').' '.$last_socket_error;

                    $this->set_error('error', $error_msg);

                    socket_close($socket);

                    return FALSE;
                }

                preg_match_all('/report=\"(.*?)\"/', $output, $found);

                $reports[$cs_name . '|' . $cs_id] = $found[1];
            }
        }

        socket_close($socket);

        return $reports;
    }


    /**
    * This function imports a report
    *
    * @param string  $report  Report path
    *
    * @return boolean
    */
    public function import_scan($report)
    {
        $this->scan = '';

        $socket = socket_create(AF_INET, SOCK_STREAM, 0);
        if ($socket < 0)
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $error_msg = _('Warning! Unable to create a connection with frameworkd. Reason:').' '.$last_socket_error;

            $this->set_error('warning', $error_msg);

            socket_close($socket);

            return FALSE;
        }

        // connect
        socket_set_block($socket);
        socket_set_option($socket,SOL_SOCKET,SO_RCVTIMEO,$this->f_config['timeout']);
        socket_set_option($socket,SOL_SOCKET,SO_SNDTIMEO,$this->f_config['timeout']);
        $result = @socket_connect($socket, $this->f_config['address'], $this->f_config['port']);

        if (!$result)
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $error_msg = _("Warning! Can't connect with frameworkd").' ('.$this->f_config['address'].':'.$this->f_config['port'].')';
            $error_msg .= ' '._('Reason').': '.$last_socket_error;

            $this->set_error('warning', $error_msg);

            socket_close($socket);

            return FALSE;
        }


        $scanning_sensor_ip = $this->sensor_cache[$this->scanning_sensor];

        //Request report results
        $in         = 'control action="nmap_report_raw_get" id="'.$scanning_sensor_ip.'" path="'.$report.'"' . "\n";
        $out        = '';
        $xml_output = '';
        $output     = '';
        $exit       = FALSE;

        socket_write($socket, $in, strlen($in));

        while (!$exit && ($out = @socket_read($socket, 2048, PHP_BINARY_READ)) !== FALSE)
        {
            $output .= $out;
            if (preg_match('/ackend$/',rtrim($out)))
            {
                $exit = TRUE;
            }
        }

        $output = trim($output);

        if (!preg_match('/ack(end)?$/', $output))
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $out = (empty($output)) ? _('empty response') : $output;

            $error_msg = _('Error! Bad response from frameworkd').": $out. "._('Reason:').' '.$last_socket_error;

            $this->set_error('error', $error_msg);

            socket_close($socket);

            return FALSE;
        }

        $lines = explode("\n", $output);

        foreach($lines as $line)
        {
            if(preg_match('/line=\"(.*)\"/',  $line, $found))
            {
                $data     = $found[1];
                $data_bin = Util::hex2bin($data);

                $xml_output .= trim(gzuncompress($data_bin))."\n";
            }
        }

        $this->scan = $xml_output;

        // show results
        $this->print_scan();

        socket_close($socket);

        return TRUE;
    }


    /**
    * This function deletes a report
    *
    * @param string  $report  Report path
    *
    * @return boolean
    */
    public function delete_scan($report)
    {
        // prepare to launch nmap
        $socket = socket_create(AF_INET, SOCK_STREAM, 0);
        if ($socket < 0)
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $error_msg = _('Warning! Unable to create a connection with frameworkd. Reason:').' '.$last_socket_error;

            $this->set_error('warning', $error_msg);

            socket_close($socket);

            return FALSE;
        }

        /* connect */
        socket_set_block($socket);
        socket_set_option($socket,SOL_SOCKET,SO_RCVTIMEO,$this->f_config['timeout']);
        socket_set_option($socket,SOL_SOCKET,SO_SNDTIMEO,$this->f_config['timeout']);
        $result = @socket_connect($socket, $this->f_config['address'], $this->f_config['port']);
        if (!$result)
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $error_msg = _("Warning! Can't connect with frameworkd").' ('.$this->f_config['address'].':'.$this->f_config['port'].')';
            $error_msg .= ' '._('Reason').': '.$last_socket_error;

            $this->set_error('warning', $error_msg);

            socket_close($socket);

            return FALSE;
        }

        $scanning_sensor_ip = $this->sensor_cache[$this->scanning_sensor];

        $in  = 'control action="nmap_report_delete" id="'.$scanning_sensor_ip.'" path="'.$report.'"' . "\n";
        $out = '';
        socket_write($socket, $in, strlen($in));
        $out = @socket_read($socket, 2048, PHP_BINARY_READ);

        if (!preg_match('/ack(end)?$/', $out))
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $out = (empty($out)) ? _('empty response') : $out;

            $error_msg = _('Error! Bad response from frameworkd').": $out. "._('Reason:').' '.$last_socket_error;

            $this->set_error('error', $error_msg);

            socket_close($socket);

            return FALSE;
        }

        socket_close($socket);

        return TRUE;
    }


    /**
    * This function prints the scan results
    *
    * @return void
    */
    public function print_scan()
    {
        $results = $this->nmap_parser->parse_xml($this->scan);

        foreach($results['scanned_ips'] as $host)
        {
            $ip = $host['ip'];

            if (trim($ip) == '')
            {
                continue;
            }

            $resolv_ip = gethostbyaddr($ip);

            echo '<img src="../pixmaps/arrow-315-small.png" align="absmiddle"/>';
            echo "<b>$ip";

            if ($ip != $resolv_ip)
            {
                echo " ($resolv_ip)";
            }

            echo '</b>.. <br/>\n';

            if (isset($host['os']))
            {
                echo '&nbsp;&nbsp;&nbsp;&nbsp;';
                echo 'OS: ' . $host['os'];
                echo '<br/>';
            }

            if (isset($host['mac']))
            {
                echo '&nbsp;&nbsp;&nbsp;&nbsp;';
                echo 'MAC: ' . $host['mac'].' '.($host['mac_vendor']);
                echo '<br/>';
            }
            if (isset($host['services']))
            {
                echo '&nbsp;&nbsp;&nbsp;&nbsp;';
                echo 'Services: ';
                foreach ($host['services'] as $k => $service)
                {
                    echo $k . '&nbsp;';
                }
                echo '<br/>';
            }

            echo '<hr/>';
        }
    }


    /**
    * This function gets last error
    *
    * @return string
    */
    public function get_last_error()
    {
        return $this->error;
    }


    /**
    * This function sets an error
    *
    * @param string  $severity  Error severity
    * @param string  $message   Error message
    *
    * @return string
    */
    public function set_error($severity, $message)
    {
        $this->error = array('severity' => $severity, 'data' => $message);
    }


    /**
    * This function gets the last scan
    *
    * @return string
    */
    public function save_scan()
    {
        $_SESSION['_scan'] = $this->scan;

        file_put_contents($this->xml_output, $this->scan);

        @chown($this->xml_output, $this->owner);
    }


    /**
    * This function returns the scan results
    *
    * @return array
    */
    public function get_scan()
    {
        if (isset($_SESSION['_scan']))
        {
            $data = $_SESSION['_scan'];
        }
        else
        {
            $data = $this->scan;
        }

        return $this->nmap_parser->parse_xml($data);
    }


    /**
    * This function deletes the scan results
    *
    * @return void
    */
    public function delete_data()
    {
        if (isset($_SESSION['_scan']))
        {
            unset($_SESSION['_scan']);
        }

        if (file_exists($this->xml_output))
        {
            @unlink($this->xml_output);
        }
    }
    
    
    /**
    * This function get the scan status
    *
    * @param string $scanning_sensor_ip   Only launch an scan with idm option and finish
    *
    * @return string
    */
    public function get_scan_status( $scanning_sensor_ip )
    {
        session_write_close();

        $socket = socket_create(AF_INET, SOCK_STREAM, 0);

        if ($socket < 0)
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $error_msg = _('Warning! Unable to create a connection with frameworkd. Reason:').' '.$last_socket_error;

            socket_close($socket);

            return $error_msg;
        }

        /* connect */
        socket_set_block($socket);
        socket_set_option($socket,SOL_SOCKET,SO_RCVTIMEO,$this->f_config['timeout']);
        socket_set_option($socket,SOL_SOCKET,SO_SNDTIMEO,$this->f_config['timeout']);
        $result = @socket_connect($socket, $this->f_config['address'], $this->f_config['port']);

        if (!$result)
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $error_msg = _("Warning! Can't connect with frameworkd").' ('.$this->f_config['address'].':'.$this->f_config['port'].')';
            $error_msg .= ' '._('Reason').': '.$last_socket_error;

            socket_close($socket);

            return $error_msg;
        }

        
        // Query until finish nmap job
        $in  = 'control action="nmap_status" id="'.$scanning_sensor_ip.'"' . "\n";
        $out = '';

        socket_write($socket, $in, strlen($in));
        $out = socket_read($socket, 2048, PHP_BINARY_READ);

        if (!preg_match('/ack(end)?$/',$out))
        {
            $last_socket_error = socket_strerror(socket_last_error($socket));

            $out = (empty($out)) ? _('empty response') : $out;

            $error_msg = _('Error! Bad response from frameworkd').": $out. "._('Reason:').' '.$last_socket_error;

            socket_close($socket);

            return $error_msg;
        }

        preg_match('/status=\"(.*?)\"/', $out, $found);

        $status=($found[1] > 0) ? _("Running") : _("Available");

        socket_close($socket);

        return $status;
    }

    
    /**
    * This function return all selected assets
    *
    * @param  object  $conn  DB Connection 
    * @param  string  $type  Asset type 
    *
    * @throws Exception  If there is any error on the procedure.
    *
    * @return  array.
    */ 
    public function get_selected_assets($conn, $type='asset')
    {
        Ossim_db::check_connection($conn);
        
        $sensors = array();
        
        $params  = array( Session::get_session_user() );
        
        if ($type == 'asset')
        {
            $host_perms_where = Asset_host::get_perms_where('h.', TRUE);
            $sql = "SELECT hex(hi.host_id) as id, INET6_NTOA(hi.ip) as ip,HEX(hsf.sensor_id) as sensor FROM user_component_filter uf, host h, host_ip hi, host_sensor_reference hsf WHERE uf.login=? AND h.id=hi.host_id AND uf.asset_id=hi.host_id AND hi.host_id=hsf.host_id AND uf.asset_type='asset' $host_perms_where";
        }
        elseif ($type == 'group')
        {
            $host_perms_where = Asset_host::get_perms_where('h.', TRUE);
            $sql = "SELECT hex(hi.host_id) as id, INET6_NTOA(hi.ip) as ip,HEX(hsf.sensor_id) as sensor FROM user_component_filter uf, host h, host_ip hi, host_sensor_reference hsf, host_group_reference hgr WHERE h.id=hi.host_id AND uf.login=? AND uf.asset_id=hgr.host_group_id AND hgr.host_id=hi.host_id AND hi.host_id=hsf.host_id AND uf.asset_type='group' $host_perms_where";
        }
        elseif ($type == 'network')
        {
            $net_perms_where  = Asset_net::get_perms_where('n.', TRUE);
            $sql = "SELECT hex(n.id) as id, nc.cidr as ip,HEX(nsr.sensor_id) as sensor FROM user_component_filter uf, net n, net_cidrs nc, net_sensor_reference nsr WHERE uf.login=? AND uf.asset_id=n.id AND nsr.net_id=n.id AND n.id=nc.net_id AND uf.asset_type='network' $net_perms_where";
        }
        else
        {
            return array();
        }

        $rs = $conn->Execute($sql, $params);

        if (!$rs)
        {
            Av_exception::throw_error(Av_exception::DB_ERROR, $conn->ErrorMsg());
        }
        
        while (!$rs->EOF)
        {
            $sensors[$rs->fields['sensor']][$rs->fields['ip']] = $rs->fields['id'];

            $rs->MoveNext();
        }
        
        $rs->free();
        
        $connected = $this->get_sensors();
        
        $jobs    = array();
        $already = array();
        
        // Sensor consolidation
        foreach ($sensors as $sensor_id => $asset)
        {
            $sensor = Av_sensor::get_object($conn, $sensor_id);
            $jobs[$sensor_id] = array(
                "available" => $connected[$sensor_id] ? 1 : 0,
                "status"    => $connected[$sensor_id] ? $this->get_scan_status( $sensor->get_ip() ) : _("Sensor not connected"),
                "name"      => $sensor->get_name(),
                "ip"        => $sensor->get_ip(),
                "assets"    => array()
            );
            foreach ($asset as $ip => $id)
            {
                if (!$already[$ip][$id])
                {
                    $jobs[$sensor_id]["assets"][] = array(
                        "ip" => $ip,
                        "id" => $id
                    );
                    $already[$ip][$id]++;
                }
            }
            if (empty($jobs[$sensor_id]["assets"]))
            {
                // Remove if there are no assets
                unset($jobs[$sensor_id]);
            }
        }
        
        return $jobs;
    }

}

/* End of file remote_scan.inc */
/* Location: ../include/classes/remote_scan.inc */
