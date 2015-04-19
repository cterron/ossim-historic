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
            return "<img src=\"$pixmap_dir/os/win.png\" alt=\"Windows\" />";
        } elseif (preg_match('/linux/i', $os)) {
            return "<img src=\"$pixmap_dir/os/linux.png\" alt=\"Linux\" />";
        } elseif (preg_match('/bsd/i', $os)) {
            return "<img src=\"$pixmap_dir/os/bsd.png\" alt=\"BSD\" />";
        } elseif (preg_match('/mac/i', $os)) {
            return "<img src=\"$pixmap_dir/os/mac.png\" alt=\"MacOS\" />";
        } elseif (preg_match('/sun/i', $os)) {
            return "<img src=\"$pixmap_dir/os/sunos.png\" alt=\"SunOS\" />";
        } elseif (preg_match('/solaris/i', $os)) {
            return "<img src=\"$pixmap_dir/os/sunos.png\" alt=\"Solaris\" />";
        }
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
EOF;
        echo "<th>".gettext("Host")."</th>";
        echo "<th>".gettext("Mac")."</th>";
        echo "<th>".gettext("OS")."</th>";
        echo "<th>".gettext("Services")."</th>";
        echo "<th>".gettext("Insert")."</th>";
      echo <<<EOF
      </tr>
      $html
      <tr></tr>
      <tr>
        <td colspan="5">
EOF;
          echo "<input type=\"submit\" value=\"".gettext("Update database values")."\" />";
      echo <<<EOF
        </td>
      </tr>
      <tr>
        <td colspan="5">
          <a href="../netscan/index.php?clearscan=1">Clear scan result</a>
        </td>
      </tr>
    </table>
    </form>
EOF;
    }

?>
