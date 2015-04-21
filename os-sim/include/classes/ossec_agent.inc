<?php
/**
* ossec_agent.inc
*
* File ossec_agent.inc is used to:n
*   - To manage OSSEC agents
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
* @package    ossim-framework\Ossec
* @autor      AlienVault INC
* @license    http://www.gnu.org/licenses/gpl-2.0.txt
* @copyright  2003-2006 ossim.net
* @copyright  2007-2013 AlienVault
* @link       https://www.alienvault.com/
*/


/**
* Ossec_agent Class
*
* Class for managing OSSEC agents
*
* @package    ossim-framework\Ossec
* @autor      AlienVault INC
* @copyright  2007-2013 AlienVault
* @link       https://www.alienvault.com/ Alienvault web page
*/

class Ossec_agent
{
    /**
    * Configuration path
    *
    */
    const CONF_PATH = 'ossec/agent.conf';
    

    /**
     * Class constructor
     *
     * This function sets up the class
     *
     */
    public function __construct()
    {

    }


    /**
     * This function lists all OSSEC agents
     *
     * @param string  $sensor_id   Sensor ID (Table alienvault.sensor)
     *
     * @throws Exception  If an error occurred
     *
     * @return object
     */
    public static function get_list($sensor_id)
    {
        $agents = array();

        $client   = new Alienvault_client();
        $response = $client->sensor($sensor_id)->list_ossec_agents();
        $response = @json_decode($response, TRUE);

        if (!$response || $response['status'] == 'error')
        {
            $exp_msg = $client->get_error_message($response);

            Av_exception::throw_error(Av_exception::USER_ERROR, $exp_msg);
        }

        $agents = $response['data']['agents'];

        return $agents;
    }


    /**
     * This function gets information about OSSEC agent
     *
     * @param string  $sensor_id   Sensor ID (Table alienvault.sensor)
     * @param string  $agent_id    Agent ID
     *
     * @throws Exception  If an error occurred
     *
     * @return array
     */
    public static function get_info($sensor_id, $agent_id)
    {
        $info = array();

        $client   = new Alienvault_client();
        $response = $client->sensor($sensor_id)->get_ossec_agent($agent_id);
        $response = @json_decode($response, TRUE);

        if (!$response || $response['status'] == 'error')
        {
            $exp_msg = $client->get_error_message($response);

            Av_exception::throw_error(Av_exception::USER_ERROR, $exp_msg);
        }

        if (!empty($response['data']['agent_detail'][0]))
        {
            $info = explode(',', $response['data']['agent_detail'][0]);
        }

        return $info;
    }


    /**
     * This function returns the last IP of OSSEC agent
     *
     * @param string  $sensor_id   Sensor ID (Table alienvault.sensor)
     * @param array   $agent       Array with agent information
     *
     * @throws Exception  If an error occurred
     *
     * @return string
     */
    public static function get_last_ip($sensor_id, $agent)
    {
        $last_ip = (Asset_host_ips::valid_ip($agent['ip'])) ? $agent['ip'] : '';
        
        $agent_name = trim($agent['name']);
        $agent_name = preg_replace("/[;' ]/", '', $agent_name);

        $client   = new Alienvault_client();
        $response = $client->sensor($sensor_id)->get_last_agent_ip($agent_name);
        $response = @json_decode($response, TRUE);

        if (!$response || $response['status'] == 'error')
        {
            $exp_msg = $client->get_error_message($response);

            Av_exception::throw_error(Av_exception::USER_ERROR, $exp_msg);
        }
        else
        {
            $last_ip = $response['data']['check'];
        }

        return $last_ip;
    }


    /**
     * This function returns the last scan dates (rootcheck and syscheck) which launched an OSSEC agent
     *
     * @param string   $sensor_id   Sensor ID (Table alienvault.sensor)
     * @param string   $agent_name  Agent name
     *
     * @throws Exception  If an error occurred
     *
     * @return array
     */
    public static function get_last_scans($sensor_id, $agent_name)
    {
        $date = array(
            'syscheck'  => '',
            'rootcheck' => ''
        );

        $client   = new Alienvault_client();
        $response = $client->sensor($sensor_id)->get_last_agent_scans($agent_name);
        $response = @json_decode($response, TRUE);

        if (!$response || $response['status'] == 'error')
        {
            $exp_msg = $client->get_error_message($response);

            Av_exception::throw_error(Av_exception::USER_ERROR, $exp_msg);
        }

        if (!empty($response['data']['check']['syscheck']))
        {
            $date['syscheck'] = gmdate("D M j H:i:s Y", $response['data']['check']['syscheck']);
        }

        if (!empty($response['data']['check']['rootcheck']))
        {
            $date['rootcheck'] = gmdate("D M j H:i:s Y", $response['data']['check']['rootcheck']);
        }

        return $date;
    }


    /**
     * This function gets IDM information about an OSSEC agent
     *
     * @param string   $sensor_id   Sensor ID (Table alienvault.sensor)
     * @param array    $agent_ip   Agent IP
     *
     * @throws Exception  If an error occurred
     *
     * @return array
     */
    public static function get_idm_data($sensor_id, $agent_ip)
    {
        $idm = array();

        $db      = new ossim_db();
        $conn    = $db->connect();

        $host_id = '';
        $params  = array();

        if (Asset_host_ips::valid_ip($agent_ip))
        {
            $sensor_ctxs = Av_sensor::get_ctx_by_id($conn, $sensor_id);

            if (is_array($sensor_ctxs) && !empty($sensor_ctxs))
            {
                $sensor_ctxs = array_keys($sensor_ctxs);

                foreach ($sensor_ctxs as $ctx)
                {
                    $host_ids = Asset_host::get_id_by_ips($conn, $agent_ip, $ctx);

                    if (!empty($host_ids))
                    {
                        $host_id = key($host_ids);
                        break;
                    }
                }
            }

            if (empty($host_id))
            {
                $host_ids = Asset_host::get_id_by_name($conn, $agent_ip);

                if (is_array($host_ids) && count($host_ids) == 1)
                {
                    $host_id = key($host_ids);
                }
            }

            if (!empty($host_id))
            {
                $params = array($host_id);

                $query  = "SELECT hp.value, hp.last_modified, h.hostname, HEX(hi.mac) AS mac, INET6_NTOP(hi.ip) AS ip, HEX(hi.host_id) AS host_id
                    FROM host_properties hp, host h, host_ip hi
                    WHERE h.id = hi.host_id AND hp.host_id = h.id AND hi.host_id = UNHEX(?) AND hp.property_ref = 8";

                $rs = $conn->Execute($query, $params);
                
                if (!$rs)
                {
                    $db->close();

                    Av_exception::throw_error(Av_exception::DB_ERROR, $conn->ErrorMsg());
                }
                
                if (!$rs->EOF)
                {
                    $idm_data    = @json_decode($rs->fields['value'], TRUE);
                    $user_domain = '';

                    if (is_array($idm_data) && !empty($idm_data))
                    {
                        foreach ($idm_data as $user)
                        {
                            $c_user =  explode('|', $user);

                            $user_domain .= (!empty($c_user[1]) ? $c_user[0].'@'.$c_user[1] : $c_user[0]);
                            $user_domain .=  '<br/>';
                        }
                    }

                    $idm = array(
                        'id'         => $rs->fields['host_id'],
                        'userdomain' => $user_domain,
                        'hostname'   => $rs->fields['hostname'],
                        'ip'         => $rs->fields['ip'],
                        'mac'        => $rs->fields['mac'],
                        'date'       => $rs->fields['last_modified']
                   );
                }

                $db->close();
            }
        }

        return $idm;
    }


    /**
     * This function creates a new OSSEC agent
     *
     * @param string  $sensor_id    Sensor ID (Table alienvault.sensor)
     * @param string  $agent_name   Agent name
     * @param string  $agent_ip     Ip address
     *
     * @throws Exception  If an error occurred
     *
     * @return array
     */
    public static function create($sensor_id, $agent_name, $agent_ip)
    {
        /************************************************************
        ******************** Create OSSEC Agent *********************
        *************************************************************/
        
        $new_agent = array();
        
        $agent_ip = ($agent_ip == 'any') ? '0.0.0.0/0' : $agent_ip;        
        
        $client   = new Alienvault_client();
        $response = $client->sensor($sensor_id)->create_ossec_agent($agent_name, $agent_ip);
        $response = @json_decode($response, TRUE);

        if (!$response || $response['status'] == 'error')
        {
            $exp_msg = $client->get_error_message($response);

            Av_exception::throw_error(Av_exception::USER_ERROR, $exp_msg);
        }

        if (!empty($response['data']['agent_detail'][0]))
        {
            $new_agent = explode(',', $response['data']['agent_detail'][0]);
        }

        /*************************************************************
        ***** Add Agent IP/CIDR to asset inventory automatically *****
        **************************************************************/
        
        if (Session::can_i_create_assets())
        {
            $db   = new ossim_db();
            $conn = $db->connect();

            //Get CTX
            $conf = $GLOBALS['CONF'];

            if (!$conf)
            {
                $conf = new Ossim_conf();
                $GLOBALS['CONF'] = $conf;
            }

            $default_ctx = str_replace('-', '', strtoupper($conf->get_conf('default_context_id')));

            $sensor_ctxs = Av_sensor::get_ctx_by_id($conn, $sensor_id);

            $ctx = $default_ctx;

            if (is_array($sensor_ctxs) && !empty($sensor_ctxs))
            {
                $ctx = (array_key_exists($default_ctx, $sensor_ctxs)) ? $default_ctx : key($sensor_ctxs);
            }

            //Sensors
            $sensors = array($sensor_id);

            if (Asset_host_ips::valid_ip($agent_ip))
            {
                $host_ids = Asset_host::get_id_by_ips($conn, $agent_ip, $ctx);

                // There is no host with this IP
                if (!is_array($host_ids) || empty($host_ids))
                {
                    try
                    {
                        $id   = Util::uuid();
                        $host = new Asset_host($conn, $id);
                        $host->set_name($agent_name);
                        $host->set_ctx($ctx);

                        $ips = array();
                        $ips[$agent_ip] = array(
                        'ip'  => $agent_ip,
                        'mac' => NULL
                        );

                        $host->set_ips($ips);
                        $host->set_descr('Ossec Agent');
                        $host->set_sensors($sensors);
                        $host->save_in_db($conn);
                    }
                    catch(Exception $e)
                    {
                        ;
                    }
                }
            }
            elseif (Asset_net::valid_cidr($agent_ip))
            {
                $net_ids = Asset_net::get_id_by_ips($conn, $agent_ip, $ctx);

                if (!is_array($net_ids) || empty($net_ids))
                {
                    try
                    {
                        $id  = Util::uuid();
                        $net = new Asset_net($id);
                        $net->set_name($agent_name);
                        $net->set_ctx($ctx);
                        $net->set_ips($agent_ip);
                        $net->set_descr('Ossec Agent');
                        $net->set_sensors($sensors);
                        $net->save_in_db($conn);
                    }
                    catch(Exception $e)
                    {
                        ;
                    }
                }
            }

            $db->close();
        }

        return $new_agent;
    }


    /**
     * This function deletes an OSSEC agent
     *
     * @param string  $sensor_id   Sensor ID (Table alienvault.sensor)
     * @param string  $agent_id    Agent ID
     *
     * @throws Exception  If an error occurred
     *
     * @return boolean
     */
    public static function delete($sensor_id, $agent_id)
    {
        $client   = new Alienvault_client();
        $response = $client->sensor($sensor_id)->delete_ossec_agent($agent_id);
        $response = @json_decode($response, TRUE);

        if (!$response || $response['status'] == 'error')
        {
            $exp_msg = $client->get_error_message($response);

            Av_exception::throw_error(Av_exception::USER_ERROR, $exp_msg);
        }

        return TRUE;
    }


    /**
     * This function restarts an OSSEC agent
     *
     * @param string  $sensor_id  Sensor ID (Table alienvault.sensor)
     * @param string  $agent_id   Agent ID
     *
     * @throws Exception  If an error occurred
     *
     * @return boolean
     */
    public static function restart($sensor_id, $agent_id)
    {
        $client   = new Alienvault_client();
        $response = $client->sensor($sensor_id)->restart_ossec_agent($agent_id);
        $response = @json_decode($response, TRUE);

        if (!$response || $response['status'] == 'error')
        {
            $exp_msg = $client->get_error_message($response);

            Av_exception::throw_error(Av_exception::USER_ERROR, $exp_msg);
        }

        return TRUE;
    }


    /**
     * This function runs the integrity/rootkit checking on one OSSEC agent
     *
     * @param string  $sensor_id  Sensor ID (Table alienvault.sensor)
     * @param string  $agent_id   Agent ID
     *
     * @throws Exception  If an error occurred
     *
     * @return boolean
     */
    public static function check_integrity($sensor_id, $agent_id)
    {
        $client   = new Alienvault_client();
        $response = $client->sensor($sensor_id)->check_ossec_agent_integrity($agent_id);
        $response = @json_decode($response, TRUE);

        if (!$response || $response['status'] == 'error')
        {
            $exp_msg = $client->get_error_message($response);

            Av_exception::throw_error(Av_exception::USER_ERROR, $exp_msg);
        }

        return TRUE;
    }


    /**
     * This function returns the key of OSSEC agent
     *
     * @param string  $sensor_id  Sensor ID (Table alienvault.sensor)
     * @param string  $agent_id   Agent ID
     *
     * @throws Exception  If an error occurred
     *
     * @return string
     */
    public static function get_key($sensor_id, $agent_id)
    {
        $client   = new Alienvault_client();
        $response = $client->sensor($sensor_id)->get_ossec_agent_key($agent_id);
        $response = @json_decode($response, TRUE);

        if (!$response || $response['status'] == 'error')
        {
            $exp_msg = $client->get_error_message($response);

            Av_exception::throw_error(Av_exception::USER_ERROR, $exp_msg);
        }

        return $response['data']['agent_key'];
    }


    /**
     * This function launches an OSSEC syscheck
     *
     * @param string   $sensor_id        Sensor ID (Table alienvault.sensor)
     * @param string   $agent_id         Agent ID
     * @param boolean  $only_w_registry  List modified registry entries (Windows only) or modified files for the agent
     *
     * @throws Exception  If an error occurred
     *
     * @return object
     */
    public static function launch_syscheck($sensor_id, $agent_id, $only_w_registry = FALSE)
    {
        $client   = new Alienvault_client();
        $response = $client->sensor($sensor_id)->launch_ossec_syscheck($agent_id, $only_w_registry);
        $response = @json_decode($response, TRUE);

        if (!$response || $response['status'] == 'error')
        {
            $exp_msg = $client->get_error_message($response);

            Av_exception::throw_error(Av_exception::USER_ERROR, $exp_msg);
        }

        return $response['data']['stdout'];
    }


    /**
     * This function launches an OSSEC rootcheck
     *
     * @param string  $sensor_id   Sensor ID (Table alienvault.sensor)
     * @param string  $agent_id    Agent ID
     *
     * @throws Exception  If an error occurred
     *
     * @return array
     */
    public static function launch_rootcheck($sensor_id, $agent_id)
    {
        $client   = new Alienvault_client();
        $response = $client->sensor($sensor_id)->launch_ossec_rootcheck($agent_id);
        $response = @json_decode($response, TRUE);

        $data = array();

        if (!$response || $response['status'] == 'error')
        {
            $exp_msg = $client->get_error_message($response);

            Av_exception::throw_error(Av_exception::USER_ERROR, $exp_msg);
        }

        $data = $response['data']['rootcheck'];

        return $data;
    }


    /**
     * This function downloads a preconfigured OSSEC Agent
     *
     * @param string  $sensor_id   Sensor ID (Table alienvault.sensor)
     * @param string  $agent_id    Agent ID
     * @param string  $agent_type  [Optional] Agent type (Agent for Windows or Linux)
     *
     * @throws Exception  If an error occurred
     *
     * @return string
     */
    public static function download_agent($sensor_id, $agent_id, $agent_type = 'windows')
    {
        $client   = new Alienvault_client();
        $response = $client->sensor($sensor_id)->download_ossec_agent($agent_id, $agent_type);
        $response = @json_decode($response, TRUE);

        if (!$response || $response['status'] == 'error')
        {
            $exp_msg = $client->get_error_message($response);

            Av_exception::throw_error(Av_exception::USER_ERROR, $exp_msg);
        }

        $data = $response['data']['path'];

        return $data;
    }


    /**
     * This function deploys a Windows agent on a remote host
     *
     * @param  string  $sensor_id   Sensor ID (Table alienvault.sensor)
     * @param  array   $d_data      Deployment data  (agent_name, agent_ip, user, domain, pass)
     *
     * @throws Exception  If there is a connection or API error
     *
     * @return  array
     */
    public static function deploy_windows_agent($sensor_id, $d_data)
    {
        $client = new Alienvault_client();

        $response = $client->sensor($sensor_id)->launch_ossec_deploy($d_data['agent_name'], $d_data['agent_ip'], $d_data['user'], $d_data['domain'], $d_data['pass']);
        $response = @json_decode($response, TRUE);

        if (!$response || $response['status'] == 'error')
        {
            $exp_msg = $client->get_error_message($response);

            Av_exception::throw_error(Av_exception::USER_ERROR, $exp_msg);
        }

        return $response['data'];
    }


    /**
     * This function checks deployment status (Windows agents)
     *
     * @param  string  $job_id  Job ID
     *
     * @throws Exception  If there is a connection or API error
     *
     * @return array
     */
    public static function check_deployment_status($job_id)
    {
        $client = new Alienvault_client();

        $response = $client->jobs()->get_job_status($job_id);
        $response = @json_decode($response, TRUE);

        if (!$response || $response['status'] == 'error')
        {
            $exp_msg = $client->get_error_message($response);

            Av_exception::throw_error(Av_exception::USER_ERROR, $exp_msg);
        }

        if ($response['data']['job_status'] == 'task-succeeded')
        {
            if ($response['data']['job_result'][0] === TRUE)
            {
                $data = array(
                    'status' => 'success',
                    'data'   => _('Deployment completed successfully')
                );
            }
            else
            {
                $data = array(
                    'status' => 'error',
                    'data'   => _($response['data']['job_result'][1])
                );
            }
        }
        elseif ($response['data']['job_status'] == 'task-failed' || $response['data']['job_status'] == 'task-revoked')
        {
            $data = array(
                'status' => 'error',
                'data'   => _('Error! Deployment could not be completed')
            );

            $data['data']['help'] = '<div style="text-align: left; padding-left: 10px;">'._('OSSEC installation failed. Please make sure that:').'</div>
                                 <div style="text-align: left; padding-left: 5px;">
                                    <ul>
                                        <li>'.('The target is running Windows XP or above').'.</li>
                                        <li>'.('The target is not running a Firewall blocking the incoming connections').'.</li>
                                        <li>'.('The user entered has enough permissions to install software in the target').'.</li>
                                        <li>'.('If the target is NOT a member of a Windows domain, User Account Control (UAC) must be disabled').'.</li>
                                    </ul>
                                 </div>';
        }
        else
        {
            $data = array(
                'status' => 'in_progress',
                'data'   => _('Installing OSSEC agent')
            );
        }

        return $data;
    }


    /**
     * This function returns information about last created agent within an agent set
     *
     * @return array
     */
    public static function get_last_agent($agents)
    {
        $data = array('', array());

        if (is_array($agents) && !empty($agents))
        {
            $agent_ids = array_keys($agents);
            $size = count($agent_ids);
            
            for ($i = $size; $i>=0; $i--)
            {
                if (is_numeric($agent_ids[$i]))
                {
                    $data = array($agent_ids[$i], $agents[$agent_ids[$i]]);

                    break;
                }
            }
        }

        return $data;
    }


    /**
     * This function returns all agent actions (HTML)
     *
     * @param string  $agent_id   Agent ID
     * @param array   $agent      Array with agent information
     *
     * @return string
     */
    public static function get_actions($agent_id, $agent)
    {
        $path = '/ossim/pixmaps';

        if (preg_match('/Local$/', $agent['status']) == FALSE)
        {
            $key    = "<a id='_key##".$agent_id."'><img src='$path/key--arrow.png' align='absmiddle' class='action_info' alt='"._('Extract Key')."' title='"._('Extract Key')."'/></a>";
            
            $delete = "<a id='_del##".$agent_id."'><img src='$path/delete.gif' align='absmiddle' class='action_info' alt='"._('Delete Agent')."' title='"._('Delete Agent')."'/></a>";
            
            $w_installer = "<a id='_w_installer_##".$agent_id."'><img src='$path/download_windows.png' align='absmiddle' class='action_info' alt='"._("Download preconfigured agent for Windows")."' title='"._("Download preconfigured agent for Windows")."'/></a>";

            $u_installer = "<a id='_u_installer_##".$agent_id."'><img src='$path/download_linux.png' align='absmiddle' class='action_info' alt='"._('Download preconfigured agent for UNIX')."' title='"._('Download preconfigured agent for UNIX')."'/></a>";

            $restart = "<a id='_restart##".$agent_id."'><img src='$path/clock.png' align='absmiddle' class='action_info' alt='"._('Restart Agent')."' title='"._('Restart Agent')."'/></a>";
        }
        else
        {
            $key    = "<span class='unbind'><img src='$path/key--arrow.png' align='absmiddle' class='action_info' alt='"._('Extract Key')."' title='"._('Extract Key')."'/></span>";
              
            $delete = "<span class='unbind'><img src='$path/delete.gif' align='absmiddle' class='action_info' alt='"._('Delete Agent')."' title='"._('Delete Agent')."'/></span>";

            $w_installer = "<span class='unbind'><img src='$path/download_windows.png' align='absmiddle' class='action_info' alt='"._('Download not available')."' title='"._('Download not available')."'/></span>";

            $u_installer = "<span class='unbind'>
                               <img src='$path/download_linux.png' align='absmiddle' class='action_info' alt='"._('Download not available')."' title='"._('Download not available')."'/></span>";

            $restart = "<span class='unbind'><img src='$path/clock.png' align='absmiddle' class='action_info' alt='"._('Restart Agent')."' title='"._('Restart Agent')."'/></span>";
        }


        //Automatic deployment only for Non-local agents and IPs (No CIDR and any)
        if (preg_match('/Local$/', $agent['status']) == FALSE && Asset_host_ips::valid_ip($agent['ip']))
        {
            
            $w_deployment = "<a id='_w_deployment_##".base64_encode($agent['ip'].'###'.$agent['name'])."'><img src='$path/w_installer.png' align='absmiddle' class='action_info' alt='"._('Automatic deployment for Windows')."' title='"._("Automatic deployment for Windows")."'/></a>"; 
        }
        else
        {
            $info = _("Automatic deployment not available");
            
            if (preg_match('/Local$/', $agent['status']))
            {
                $info = _('Automatic deployment not available for Local agent');
            }

            $w_deployment = "<span class='unbind'><img src='$path/w_installer.png' align='absmiddle' class='action_info' alt='".$info."' title='".$info."'/></span>";
        }


        $mfiles = "<a id='_file##".$agent_id."'><img src='$path/file.png' align='absmiddle' class='action_info' alt='"._('Modified Files')."' title='"._('Modified Files')."'/></a>";

        $mreg   = "<a id='_reg##".$agent_id."'><img src='$path/registry.gif' align='absmiddle' class='action_info' alt='"._('Modified Reg. Entries')."' title='"._('Modified Reg. Entries')."'/></a>";

        $rchk   = "<a id='_rchk##".$agent_id."'><img src='$path/radar.gif' align='absmiddle' class='action_info' alt='"._('Rootcheck')."' title='"._('Rootcheck')."'/></a>";

        $check   = "<a id='_check##".$agent_id."'><img src='$path/tick-circle.png' align='absmiddle' class='action_info' alt='"._('Integrity/rootkit checking')."' title='"._('Integrity/rootkit checking')."'/></a>";

        /*
            Note: OSSEC agent for UNIX not available yet
            return $restart.$check.$key.$mfiles.$mreg.$rchk.$w_installer.$w_deployment.$u_installer.$delete;
        */

        return $restart.$check.$key.$mfiles.$mreg.$rchk.$w_installer.$w_deployment.$delete;
    }
    

    /**
     * This function gets the agent configuration (agent.conf)
     *
     * @param string   $sensor_id   Sensor ID (Table alienvault.sensor)
     *
     * @throws Exception  If an error occurred
     *
     * @return array
     */
    public static function get_configuration_file($sensor_id)
    {
        $data = array();

        $client   = new Alienvault_client();
        $response = $client->sensor($sensor_id)->get_agent_config();
        $response = @json_decode($response, TRUE);

        if (!$response || $response['status'] == 'error')
        {
            $exp_msg = $client->get_error_message($response);

            Av_exception::throw_error(Av_exception::USER_ERROR, $exp_msg);
        }

        $conf_file = $response['data']['local_path'];

        $data['path'] = $conf_file;
        $data['data'] = @file_get_contents($conf_file);

        if ($data['data'] === FALSE)
        {
            $exp_msg = _('Error! Failed to get the agent configuration file. You do not have enough access privileges');

            Av_exception::throw_error(Av_exception::USER_ERROR, $exp_msg);
        }

        return $data;
    }


    /**
     * This function sets the new agent configuration (agent.conf)
     *
     * @param string  $sensor_id   Sensor ID (Table alienvault.sensor)
     * @param string  $new_conf    New configuration
     *
     * @throws Exception  If an error occurred
     *  
     * @return array
     */
    public static function set_configuration_file($sensor_id, $new_conf)
    {
        $data['status']  = 'success';
        $data['msg']     = _('Agent configuration file updated successfully');
        $data['data']    = $new_conf;

        $conf_file = Ossec_utilities::get_real_path($sensor_id, self::CONF_PATH);

        //Special case:  Disable email notification
        $pattern = '/<\s*email_notification\s*>\s*yes\s*<\s*\/email_notification\s*>/';

        if (preg_match($pattern, $new_conf))
        {
            $new_conf = preg_replace($pattern, '<email_notification>no</email_notification>', $new_conf);

            $system_name = (Session::is_pro()) ? 'AlienVault' : 'OSSIM';
            $warning_msg = _('OSSEC email notification is not supported. Please, use '.$system_name.' email notifications instead').'.';

            $data['status'] = 'warning';
            $data['msg']    = _('Agent configuration file updated with warnings:').'<br/><br/>'. $warning_msg;
            $data['data']   = $new_conf;
        }

        if (@file_put_contents($conf_file, $new_conf, LOCK_EX) === FALSE)
        {
            $exp_msg = _('Error! Failed to set Agent configuration file. You do not have enough access privileges');

            Av_exception::throw_error(Av_exception::USER_ERROR, $exp_msg);
        }


        $client   = new Alienvault_client();
        $response = $client->sensor($sensor_id)->set_agent_config();
        $response = @json_decode($response, TRUE);

        if (!$response || $response['status'] == 'error')
        {
            $exp_msg = $client->get_error_message($response);

            Av_exception::throw_error(Av_exception::USER_ERROR, $exp_msg);
        }

        return $data;
    }
}

/* End of file ossec_agent.inc */
/* Location: ../include/classes/ossec_agent.inc */