<?php
    require_once ('classes/Session.inc');
    Session::logcheck("MenuPolicy", "PolicyResponses");

    require_once ('ossim_db.inc');
    require_once ('classes/Action.inc');
    require_once ('classes/Response.inc');
    require_once ('classes/Host.inc');

    $db = new ossim_db();
    $conn = $db->connect();
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>Responses</h1>

  <table align="center" width="100%">
    <tr>
      <th nowrap>Description</th>
      <th nowrap>Source</th>
      <th nowrap>Dest</th>
      <th nowrap>Source Ports</th>
      <th nowrap>Dest Ports</th>
      <th nowrap>Sensors</th>
      <th nowrap>Plugins</th>
      <th nowrap>Actions</th>
      <th nowrap>#</th>
      <td></td>
    </tr>

<?php
    if (is_array($response_list = Response::get_list($conn))) {
        foreach ($response_list as $response) {
?>
    <tr>
      <!-- description -->
      <td><?php echo $response->get_descr(); ?></td>
      <!-- end description -->

      <td>
        <table class="noborder" width="100%">
          <tr>

            <!-- source nets -->
            <td class="noborder">
        <?php
        if (is_array($source_net_list = $response->get_source_nets($conn))) {
            foreach ($source_net_list as $net) {
                echo gettext("Net ") . $net->get_net()."<br/>";
            }
        }
        ?>
            </td>
            <!-- end source nets -->

            <!-- source hosts -->
            <td class="noborder">
        <?php
        if (is_array($source_host_list = $response->get_source_hosts($conn))) {
            foreach ($source_host_list as $host) {
                echo gettext("Host ") . 
                    Host::ip2hostname($conn, $host->get_host())."<br/>";
            }
        }
        ?>
            </td>
            <!-- end source hosts -->

          </tr>
        </table>
      </td>

      <td>
        <table class="noborder" width="100%">
          <tr>

            <!-- dest nets -->
            <td class="noborder">
        <?php
        if (is_array($dest_net_list = $response->get_dest_nets($conn))) {
            foreach ($dest_net_list as $net) {
                echo gettext("Net ") . $net->get_net()."<br/>";
            }
        }
        ?>
            </td>
            <!-- end dest nets -->

            <!-- dest hosts -->
            <td class="noborder">
        <?php
        if (is_array($dest_host_list = $response->get_dest_hosts($conn))) {
            foreach ($dest_host_list as $host) {
                echo gettext("Host ") . 
                    Host::ip2hostname($conn, $host->get_host())."<br/>";
            }
        }
        ?>
            </td>
            <!-- end dest hosts -->
          </tr>
        </table>
      </td>

      <!-- source ports -->
      <td>
        <?php
        if (is_array($source_ports_list = $response->get_source_ports($conn))) {
            foreach ($source_ports_list as $port) {
                if ($port->get_port() == 0)
                    echo "ANY";
                else
                    echo $port->get_port() . "<br/>";
            }
        }
        ?>
      </td>
      <!-- end source ports -->

      <!-- dest ports -->
      <td>
        <?php
        if (is_array($dest_ports_list = $response->get_dest_ports($conn))) {
            foreach ($dest_ports_list as $port) {
                if ($port->get_port() == 0)
                    echo "ANY";
                else
                    echo $port->get_port() . "<br/>";
            }
        }
        ?>
      </td>
      <!-- end dest ports -->

      <!-- sensors -->
      <td>
        <?php
        if (is_array($sensor_list = $response->get_sensors($conn))) {
            foreach ($sensor_list as $sensor) {
                echo Host::ip2hostname($conn, $sensor->get_host())."<br/>";
            }
        }
        ?>
      </td>
      <!-- end sensors -->

      <!-- plugins -->
      <td>
        <?php
        if (is_array($plugin_list = $response->get_plugins($conn))) {
            foreach ($plugin_list as $plugin) {
                if ($plugin->get_plugin_id() == 0)
                    echo "ANY";
                else
                    echo $plugin->get_plugin_id()."<br/>";
            }
        }
        ?>
      </td>
      <!-- end plugins -->

      <!-- actions -->
      <td>
        <?php
        if (is_array($action_list = $response->get_actions($conn))) {
            foreach ($action_list as $action) {
                $a = Action::get_action_by_id($conn, $action->get_action_id());
                echo $a->get_descr()."<br/>";
            }
        }
        ?>
      </td>
      <!-- end actions -->

      <td>
        <a href="deleteresponse.php?id=<?php echo $response->get_id() ?>">Delete</a>
      </td>

    </tr>
<?php
        }
    }
?>
    <tr>
      <td colspan="9">
        <a href="newresponseform.php">Insert new response</a>
      </td>
    </tr>


<?php
/*
    print '<a href="newresponseform.php">New Response</a>';

    print "<pre>";
    print "Response:<br/>";
    print_r(Response::get_list($conn));
    print "Response Source Hosts:<br/>";
    print_r(Response::get_source_hosts($conn));
    print "Response Source Nets:<br/>";
    print_r(Response::get_source_nets($conn));
    print "Response Dest Hosts:<br/>";
    print_r(Response::get_dest_hosts($conn));
    print "Response Dest Nets:<br/>";
    print_r(Response::get_dest_nets($conn));
    print "Response Source Ports:<br/>";
    print_r(Response::get_source_ports($conn));
    print "Response Dest Ports:<br/>";
    print_r(Response::get_dest_ports($conn));
    print "Response Sensors:<br/>";
    print_r(Response::get_sensors($conn));
    print "Response Plugins:<br/>";
    print_r(Response::get_plugins($conn));
    print "Response Actions:<br/>";
    print_r(Response::get_actions($conn));
    print "</pre>";
*/
?>


<?php
    $db->close($conn);
?>

</body>
</html>

