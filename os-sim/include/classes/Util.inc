<?php

require_once ('classes/Host.inc');

class Util {

    /* Expects a string with underscore in it and substitutes with spaces.
       Used for DB Integrity */
    function beautify($name)
    {
        return str_replace("_", " ", $name);
    }

    // Translates the string into human readable form
    function translate_alarm($conn, $name, $alarm)
    {
        $translations = array (   
                "/SRC_IP/"      =>  "get_src_ip()", 
                "/DST_IP/"      =>  "get_dst_ip()",
                "/SRC_PORT/"    =>  "get_src_port()", 
                "/DST_PORT/"    =>  "get_dst_port()",
                "/PROTOCOL/"    =>  "get_protocol()", 
                "/PLUGIN_ID/"   =>  "get_plugin_id()",
                "/PLUGIN_SID/"  =>  "get_plugin_sid()" 
            );

        foreach ($translations as $k => $v ) 
        {
            $pattern = '$name = preg_replace("'.$k.'", %s, $name);';

            switch($k) {
                case "/PROTOCOL/":
                    $replacement = 'getprotobynumber($alarm->'.$v.')';
                    break;
                case "/SRC_IP/":
                case "/DST_IP/":
                    $replacement = 'Host::ip2hostname($conn, $alarm->'.$v.')';
                    break;
                default :
                    $replacement = '$alarm->'.$v;
                }
                $str = sprintf($pattern, $replacement);
                eval($str);
        }
        return $name;
    }

    function timestamp2date($timestamp)
    {
        if (!$timestamp) return "";
        
        $num = "[0-9]";

        if (preg_match("/^$num{14}$/", $timestamp)) 
        {
            /* 
                MySQL < 4.1:
                TIMESTAMP is returned as a string in 'YYYYMMDDHHMMSS' 
            */
            return (substr($timestamp, 0, 4)  . '-' .  # YYYY
                    substr($timestamp, 4, 2)  . '-' .  # MM
                    substr($timestamp, 6, 2)  . ' ' .  # DD
                    substr($timestamp, 8, 2)  . ':' .  # HH
                    substr($timestamp, 10, 2) . ':' .  # MM
                    substr($timestamp, 12, 2));        # SS
        }
        
        elseif (preg_match 
                    ("/$num{4}\-$num{2}\-$num{2} $num{2}\:$num{2}\:$num{2}/", 
                     $timestamp)) 
        {
            /* 
                MySQL >= 4.1:
                TIMESTAMP is returned as a string in 'YYYY-MM-DD HH:MM:SS' 
            */
            return $timestamp;
        }

        else {
            return $timestamp;
        }
    }
}

?>
