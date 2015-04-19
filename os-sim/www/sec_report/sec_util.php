<?php

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
