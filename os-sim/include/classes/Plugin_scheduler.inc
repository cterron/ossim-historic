<?php
require_once('classes/Log_action.inc');
require_once('classes/Plugin_scheduler_sensor_reference.inc');
require_once('classes/Plugin_scheduler_netgroup_reference.inc');
require_once('classes/Plugin_scheduler_hostgroup_reference.inc');
require_once('classes/Plugin_scheduler_host_reference.inc');
require_once('classes/Plugin_scheduler_net_reference.inc');
require_once ("ossim_db.inc");

class Plugin_scheduler {

    var $id;
    var $plugin;
    var $minute;
    var $hour;
    var $day_month;
    var $month;
    var $day_week;
    var $type_scan;

    function Plugin_scheduler ($id, $plugin, $minute, $hour, $day_month, $month, $day_week, $type_scan)
    {
	$this->id		= $id;
	$this->plugin		= $plugin;
	$this->minute		= $minute;
	$this->hour		= $hour;
	$this->day_month	= $day_month;
	$this->month		= $month;
	$this->day_week		= $day_week;
	$this->type_scan	= $type_scan;
    }

    function get_id()          		{ return $this->id;		}
    function get_plugin()          	{ return $this->plugin;		}
    function get_minute()          	{ return $this->minute;        	}
    function get_hour()       		{ return $this->hour;        	}
    function get_day_month()          	{ return $this->day_month;      }
    function get_month()          	{ return $this->month;        	}
    function get_day_week()          	{ return $this->day_week;       }
    function get_type_scan()            { return $this->type_scan;      }

    function get_list($conn, $args = "") 
    {

	$list = array();
        $query = OssimQuery("SELECT * FROM plugin_scheduler $args");

        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            $list = array();
            while (!$rs->EOF) {
                $list[] = new Plugin_scheduler ($rs->fields["id"],
                                         $rs->fields["plugin"], 
                                         $rs->fields["plugin_minute"], 
                                         $rs->fields["plugin_hour"], 
                                         $rs->fields["plugin_day_month"], 
                                         $rs->fields["plugin_month"], 
                                         $rs->fields["plugin_day_week"],
    					 $rs->fields["type_scan"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function get_sensors($conn, $id) {
        return Plugin_scheduler_sensor_reference::get_list($conn, $id);
    }
    
    function get_netgroups($conn, $id) {
	return Plugin_scheduler_netgroup_reference::get_list($conn, $id);
    }
    
    function get_hostgroups($conn, $id) {
	return Plugin_scheduler_hostgroup_reference::get_list($conn, $id);
    }

    function get_nets($conn, $id){
	return Plugin_scheduler_net_reference::get_list($conn, $id); 
    }    

    function get_hosts($conn, $id){
	return Plugin_scheduler_host_reference::get_list($conn, $id);
    }

    function insert($conn, $plugin, $minute, $hour, $day_month, $month, $day_week, $sensor_list, $netgroup_list, $hostgroup_list, $net_list, $host_list, $type_scan, $id = null)
    {

      	if (!$id) {
            $id = $conn->GenID('plugin_scheduler_seq');
        }


        $sql = "INSERT INTO plugin_scheduler VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
        $params = array($id, $plugin, $minute, $hour, $day_month, $month, $day_week, $type_scan);
        
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }

	if ($sensor_list != null){
	    Plugin_scheduler_sensor_reference::insert($conn, $sensor_list, $id);
        }

	if ($netgroup_list != null){
       	    Plugin_scheduler_netgroup_reference::insert($conn, $netgroup_list, $id);
	 }

	if ($hostgroup_list != null){
	    Plugin_scheduler_hostgroup_reference::insert($conn, $hostgroup_list, $id);
        }

	if ($net_list != null){
            Plugin_scheduler_net_reference::insert($conn, $net_list, $id);
        }

	if ($host_list != null){
            Plugin_scheduler_host_reference::insert($conn, $host_list, $id);
        }
    }
    
    function delete($conn, $id) {
        $sql = "DELETE FROM plugin_scheduler WHERE id  = ?";
        $params = array($id);
        if ($conn->Execute($sql, $params) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
	    exit;
        }
        Plugin_scheduler_sensor_reference::delete($conn, $id);
    }
}


?>
