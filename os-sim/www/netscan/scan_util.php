<?php

    /*
     * scan_util.php
     *
     * methods not included in Scan.inc go here.
     */


    function __os2pixmap($os) 
    {
        $pixmap_dir = "../pixmaps/";

        if (preg_match('/win/i', $os)) {
            return "<img src=\"$pixmap_dir/os/win.png\" alt=\"win\" />";
        } elseif (preg_match('/linux/i', $os)) {
            return "<img src=\"$pixmap_dir/os/linux.png\" alt=\"linux\" />";            } elseif (preg_match('/bsd/i', $os)) {
            return "<img src=\"$pixmap_dir/os/bsd.png\" alt=\"bsd\" />";
        } elseif (preg_match('/mac/i', $os)) {
            return "<img src=\"$pixmap_dir/os/mac.png\" alt=\"mac\" />";
        } elseif (preg_match('/sun|solaris/i', $os)) {
            return "<img src=\"$pixmap_dir/os/sunos.png\" alt=\"sunos\" />";            }
    }

    function scan2html($scan)
    {
        $count = 0;
        $html = "<br/>";
        foreach ($scan as $host) {
            $html .= "<tr>";
            $html .= "<td>" . $host['ip']   . "</td>\n";
            $html .= "<td>" . $host['mac'];
            $html .= "&nbsp;" . $host['mac_vendor'] . "</td>\n";
            $html .= "<td>" . $host['os'] . "&nbsp;";
            $html .= __os2pixmap($host['os']) . "&nbsp;</td>\n";
            $html .= "<td>";
            foreach ($host["services"] as $service) {
                $title = $service["port"] . "/" . 
                    $service["proto"] . " " .
                    $service["version"];
                $html .= " <span title=\"$title\"> ";
                $html .= $service["service"];
                $html .= "</span>&nbsp;";
            }
            $html .= "&nbsp</td>\n";
            $html .= "<td><input CHECKED type=\"checkbox\" 
                value=\"". $host['ip'] . "\" name=\"ip_$count\"/></td>\n";
            $html .= "</tr>";
            $count += 1;
        }
        
        echo <<<EOF
    <form action="../host/newhostform.php" method="POST">
      <input type="hidden" name="scan" value="1" />
      <input type="hidden" name="ips" value="$count" />
    <table align="center">
      <tr>
        <th>Host</th>
        <th>Mac</th>
        <th>OS</th>
        <th>Services</th>
        <th>Insert</th>
      </tr>
      $html
      <tr></tr>
      <tr>
        <td colspan="5">
          <input type="submit" value="Update database values" />
        </td>
      </tr>
    </table>
    </form>
EOF;
    }

?>
