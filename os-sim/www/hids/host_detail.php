<?php
//require_once ('classes/Session.inc');
//Session::logcheck("MenuConfiguration", "ConfigurationHostScan");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Host IDS</h1>

<?php

    $ip = $_GET["ip"];
    $date = $_GET["date"];
    require_once ('ossim_db.inc');
    
    $db = new ossim_db();
    $conn = $db->connect();
?>
<table>
<?php 

        $query = "SELECT *, inet_ntoa(ip) FROM host_ids where ip = inet_aton('$ip') and date = '$date'";

        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
        $ip = $rs->fields["inet_ntoa(ip)"];
        $sid = $rs->fields["sid"];
        $what = $rs->fields["what"];
        $event_type = $rs->fields["event_type"];
        $target = $rs->fields["target"];
        $extra_data = $rs->fields["extra_data"];
        printf("<TR><TD>
        $ip</TD><TD>
        $sid</TD><TD>
        $what</TD><TD>
        $event_type</TD><TD>
        $target</TD><TD>
        $extra_data</TD><TD></TR>");
                $rs->MoveNext();            
            }
        }        
?>
</table>
</body>
</html>
<?php

$db->close($conn);
exit();
?>
