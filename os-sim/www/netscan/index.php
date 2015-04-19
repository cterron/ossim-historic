<?php

    // menu authentication
    require_once ('classes/Session.inc');
    Session::logcheck("MenuTools", "ToolsScan");

    // Get a list of nets from db
    require_once ("ossim_db.inc");
    $db = new ossim_db();
    $conn = $db->connect();

    require_once ("classes/Net.inc");
    $net_list = Net::get_list($conn);

    $db->close($conn);

?>


<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>

  <script>
    // enable text input when manual option is selected
    function check_change() {
        form = document.forms['net_form'];
        if (form.net.value != '')
            form.net_input.disabled = true;
        else
            form.net_input.disabled = false;
        form.net_input.value = form.net.value;
    }
  </script>
  
</head>

<body>
  <h1> <?php echo gettext("Net Scan") ?> </h1>


  <!-- net selector form -->
  <form name="net_form" method="GET" action="do_scan.php">
  <table align="center">
    <tr>
      <td colspan="3">
        <?php echo gettext("Please, select the network you want to scan:") ?>
      </td>
    </tr>
    <tr>
      <td>
        <select name="net" onChange="javascript:check_change()">
<?php
    if (is_array($net_list)) {
        $first_net = $net_list[0]->get_ips();
        foreach ($net_list as $net) {
            
?>
          <option name="<?php echo $net->get_name() ?>" 
                  value="<?php echo $net->get_ips() ?>">
            <?php echo $net->get_name() ?>
          </option>
<?php
        }
    }
?>
          <option name="manual" value="">Manual</option>
        </select>
      </td>
      <td><input type="text" value="<?php echo $first_net ?>" 
                 name="net_input" disabled /></td>
      <td><input type="submit" value="<?php echo gettext("Scan") ?>" /></td>
    </tr>
  </table>
  </form>
  <!-- end of net selector form -->

<?php

    require_once ('classes/Scan.inc');
    $scan = Scan::get_scan();

    if (is_array($scan)) {
        require_once ('scan_util.php');
        scan2html ($scan);
    } else {
        echo "<p align=\"center\">";
        echo gettext("NOTE: This tool is a nmap frontend. In order to use all
            nmap funcionality, you need root privileges.");
        echo "<br/>";
        echo gettext("For this purpose you can use suphp, or set suid 
            to nmap binary (chmod 4755 /usr/bin/nmap)");
        echo "</p>";
    }

?>


</body>
</html>
