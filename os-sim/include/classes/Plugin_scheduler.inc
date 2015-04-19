<?php
require_once('classes/Log_action.inc');
require_once('classes/Plugin_scheduler_sensor_reference.inc');
require_once ("ossim_db.inc");

class Plugin_scheduler {

    var $id;
    var $plugin;
    var $minute;
    var $hour;
    var $day_month;
    var $month;
    var $day_week;

    function Plugin_scheduler ($id, $plugin, $minute, $hour, $day_month, $month, $day_week)
    {
	$this->id		= $id;
	$this->plugin		= $plugin;
	$this->minute		= $minute;
	$this->hour		= $hour;
	$this->day_month	= $day_month;
	$this->month		= $month;
	$this->day_week		= $day_week;
    }

    function get_id()          		{ return $this->id;		}
    function get_plugin()          	{ return $this->plugin;		}
    function get_minute()          	{ return $this->minute;        	}
    function get_hour()       		{ return $this->hour;        	}
    function get_day_month()          	{ return $this->day_month;      }
    function get_month()          	{ return $this->month;        	}
    function get_day_week()          	{ return $this->day_week;       }

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
                                         $rs->fields["plugin_day_week"]);
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function get_sensors($conn, $id) {
        return Plugin_scheduler_sensor_reference::get_list($conn, $id);
    }
    
    function insert($conn, $plugin, $minute, $hour, $day_month, $month, $day_week, $sensor_list, $id = null)
    {

      	if (!$id) {
            $id = $conn->GenID('plugin_scheduler_seq');
        }


        $sql = "INSERT INTO plugin_scheduler VALUES (?, ?, ?, ?, ?, ?, ?)";
        $params = array($id, $plugin, $minute, $hour, $day_month, $month, $day_week);
        
        if ($conn->Execute($sql, $params) === false) {
            print 'error inserting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
        Plugin_scheduler_sensor_reference::insert($conn, $sensor_list, $id);
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
