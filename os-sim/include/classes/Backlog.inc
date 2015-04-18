<?php

require_once ('classes/Conf.inc');

class Backlog {

    var $utime;
    var $id;
    var $name;
    var $rule_level;
    var $rule_type;
    var $rule_name;
    var $occurrence;
    var $time_out;
    var $matched;
    var $plugin_id;
    var $plugin_sid;
    var $src_ip;
    var $dst_ip;
    var $src_port;
    var $dst_port;
    var $condition;
    var $value;
    var $time_interval;
    var $absolute;
    var $priority;
    var $reliability;

    function Backlog ($utime, $id, $name, $rule_level, $rule_type, $rule_name,
                      $occurrence, $time_out, $matched, $plugin_id, 
                      $plugin_sid, $src_ip, $dst_ip, $src_port, $dst_port,
                      $condition, $value, $time_interval, $absolute, 
                      $priority, $reliability)
    {
        $this->utime            =$utime;         
        $this->id               =$id;            
        $this->name             =$name;          
        $this->rule_level       =$rule_level;    
        $this->rule_type        =$rule_type;     
        $this->rule_name        =$rule_name;     
        $this->occurrence       =$occurrence;    
        $this->time_out         =$time_out;      
        $this->matched          =$matched;       
        $this->plugin_id        =$plugin_id;     
        $this->plugin_sid       =$plugin_sid;    
        $this->src_ip           =$src_ip;        
        $this->dst_ip           =$dst_ip;        
        $this->src_port         =$src_port;      
        $this->dst_port         =$dst_port;      
        $this->condition        =$condition;     
        $this->value            =$value;         
        $this->time_interval    =$time_interval; 
        $this->absolute         =$absolute;      
        $this->priority         =$priority;      
        $this->reliability      =$reliability;   
    }

    function get_utime()            { return $this->utime;         }
    function get_id()               { return $this->id;            }
    function get_name()             { return $this->name;          }
    function get_rule_level()       { return $this->rule_level;    }
    function get_rule_type()        { return $this->rule_type;     }
    function get_rule_name()        { return $this->rule_name;     }
    function get_occurrence()       { return $this->occurrence;    }
    function get_time_out()         { return $this->time_out;      }
    function get_matched()          { return $this->matched;       }
    function get_plugin_id()        { return $this->plugin_id;     }
    function get_plugin_sid()       { return $this->plugin_sid;    }
    function get_src_ip()           { return $this->src_ip;        }
    function get_dst_ip()           { return $this->dst_ip;        }
    function get_src_port()         { return $this->src_port;      }
    function get_dst_port()         { return $this->dst_port;      }
    function get_condition()        { return $this->condition;     }
    function get_value()            { return $this->value;         }
    function get_time_interval()    { return $this->time_interval; }
    function get_absolute()         { return $this->absolute;      }
    function get_priority()         { return $this->priority;      }
    function get_reliability()      { return $this->reliability;   }

    function get_count($conn) 
    {
        $query = "SELECT count(*) FROM backlog;";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            return $rs->fields["count(*)"];
        }
    }

    function get_list($conn, $args = "", $inf = 0, $sup = 25) 
    {
        $i = 0;

        $query = "SELECT *, inet_ntoa(src_ip), inet_ntoa(dst_ip) 
            FROM backlog $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                if (($i++ >= $inf) && ($inf++ <= $sup)) {
                    $list[] = new Backlog ($rs->fields["utime"], 
                                           $rs->fields["id"],
                                           $rs->fields["name"],
                                           $rs->fields["rule_level"],
                                           $rs->fields["rule_type"],
                                           $rs->fields["rule_name"],
                                           $rs->fields["ocurrences"],
                                           $rs->fields["time_out"],
                                           $rs->fields["matched"],
                                           $rs->fields["plugin_id"],
                                           $rs->fields["plugin_sid"],
                                           $rs->fields["inet_ntoa(src_ip)"],
                                           $rs->fields["inet_ntoa(dst_ip)"],
                                           $rs->fields["src_port"],
                                           $rs->fields["dst_port"],
                                           $rs->fields["condition"],
                                           $rs->fields["value"],
                                           $rs->fields["time_interval"],
                                           $rs->fields["absolute"],
                                           $rs->fields["priority"],
                                           $rs->fields["reliability"]);
                } elseif ($inf > $sup) {
                    break;
                }
                $rs->MoveNext();
            }
        }
        return $list;
    }

    function delete($conn, $utime) {
        if (!strcmp($utime, "all")) {
            $query = "DELETE FROM backlog;";
        } else {
            $query = "DELETE FROM backlog WHERE utime = '$utime';";
        }
        if ($conn->Execute($query) === false) {
            print 'error deleting: '.$conn->ErrorMsg().'<BR>';
            exit;
        }
    }

}


?>
