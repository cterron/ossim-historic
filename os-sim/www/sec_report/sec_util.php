<?php

    require_once ('classes/Host.inc');
    require_once ('classes/Port.inc');

    function ip2hostname($ip) {

        $db = new ossim_db();
        $conn = $db->connect();
        $hostname = Host::ip2hostname($conn, $ip);
        $db->close($conn);

        return $hostname;
    }

    function port2service($port) {
        $db = new ossim_db();
        $conn = $db->connect();
        if ($port_list = Port::get_list($conn, "WHERE port_number = $port")) {
            return $port_list[0]->get_service();
        } else {
            return "";
        }
    }

    function get_os_pixmap($ip) {
        $db = new ossim_db();
        $conn = $db->connect();
        $os = Host_os::get_os_pixmap($conn, $ip);
        $db->close($conn);
        
        return $os;
    }

    function echo_risk($risk)
    {
        $width = (20 * $risk) + 1;
        $img = "<img src=\"../pixmaps/gauge-yellow.jpg\" width=\"$width\" height=\"15\" />";
        
        if ($risk  > 7) {
            $img = "<img src=\"../pixmaps/gauge-red.jpg\" " . 
                "width=\"$width\" height=\"15\" />";
            echo "<td nowrap class=\"left\">$img $risk</td>";
        } elseif ($risk > 4) {
            $img = "<img src=\"../pixmaps/gauge-yellow.jpg\" " . 
                "width=\"$width\" height=\"15\" />";
            echo "<td nowrap class=\"left\">$img $risk</td>";
        } elseif ($risk > 2) {
            $img = "<img src=\"../pixmaps/gauge-green.jpg\" " . 
                "width=\"$width\" height=\"15\" />";
            echo "<td nowrap class=\"left\">$img $risk</td>";
        } else {
            $img = "<img src=\"../pixmaps/gauge-blue.jpg\" " . 
                "width=\"$width\" height=\"15\" />";
            echo "<td nowrap class=\"left\">$img $risk</td>";
        }
    }
?>
