<?php

    /*
     * net argument in nmap format:
     * example: ?net=192.168.1.1-255
     */
    if (!$net = escapeshellcmd($_GET["net"])) {
        echo "No net selected";
        exit;
    }

    /* 
     * get conf 
     * needed to get nmap path
     */
    require_once ('ossim_conf.inc');
    $conf = new ossim_conf();
    $nmap = $conf->get_conf("nmap_path");
    
    /*
     * connect to db
     * needed to get ntop links associated with hosts
     */
    require_once ('ossim_db.inc');
    $db = new ossim_db();
    $conn = $db->connect();

    require_once ('classes/Host.inc');
    require_once ('classes/Host_os.inc');

    /* 
     * convert net argument into an array of hosts
     */
    $ip_string = 
        shell_exec("$nmap -n -sL $net | grep Host | cut -f 2 -d \" \" ");
    $ip_list = explode("\n", $ip_string);
    array_pop($ip_list);
    


    $found = 0;  /* tcp session found in html page */
    $show = 0;   /* begin of print output  */

    foreach ($ip_list as $host)
    {
        /* 
         * get ntop link associated with host 
         */
        $ntop_link = ossim_db::get_sensor_link($conn, $host);
        
        if ($fd = @fopen("$ntop_link/$host.html", "r"))
        {
            while (!feof ($fd))
            {
                $line = fgets ($fd, 1024);

                /* 
                 * search for Sessions section 
                 */
                if (eregi ("<title>Active TCP Sessions</title>", $line)) {
                    $found = 1;
                }

                /* 
                 * begin to print at the begin of <table>...
                 */
                if ($found && eregi('<table', $line)) {
                    $show = 1;
                    $hostname = Host::ip2hostname($conn, $host);
                    $os_pixmap = Host_os::get_os_pixmap($conn, $host);
                    if (strcmp($hostname, $host)) $hostname .= " ($host)";
                    echo <<<EOF
<HTML>
  <HEAD>
    <TITLE>Active TCP Sessions</TITLE>
    <LINK REL=stylesheet HREF="$ntop_link/style.css" type="text/css">
  </HEAD>
  <BODY>
    <H2 align="center">
      <a href="../report/index.php?section=usage&host=$host">$hostname</a>
      $os_pixmap
    </H2>
EOF;
                }

                /* 
                 * </table> found, session section finished, stop printing 
                 */
                if ($found && eregi('</table', $line)) {
                    $show = 0;
                    $found = 0;
                    echo <<<EOF
    </TABLE>
    <BR/>
  </BODY>
</HTML>
EOF;
                }

                /* 
                 * print data, adjusting links
                 */
                if ($show && $found) {
                    $line = ereg_replace ("<img src=\"", 
                                          "<img src=\"$ntop_link", $line);
                    $line = ereg_replace ("<a href=\"", 
                                          "<a href=\"$ntop_link", $line);
                    echo $line;
                }

            }

            /*
             * next host!
             */
            fclose($fd);
            $found = 0;
            $show = 0;
        }
    }

    $db->close($conn);
?>

