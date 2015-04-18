<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuReports", "ReportsIncidents");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>Incidents</h1>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Incident.inc';

    if (!$order = $_GET["order"]) $order = "id DESC";

    $db = new ossim_db();
    $conn = $db->connect();


    /* filter */
    $where = "";
    if ($_GET["type"] or $_GET["title"])
    {
        if ($_GET["type"])
            $where .= "ref = '" . $_GET["type"] . "' ";
        if ($_GET["title"]) {
            if ($where) $where .= "AND ";
            $where .= "title LIKE '%" . $_GET["title"] . "%'";
        }
        $where = "WHERE " . $where;
    }

?>

  <!-- filter -->
  <form method="GET" action="<?php echo $_SERVER["PHP_SELF"] ?>">
  <table align="center">
    <tr>
      <th colspan="6">Filter</th>
    </tr>
    <tr>
      <td>Type</td>
      <td>Title</td>
      <td>In charge</td>
      <td>Status</td>
      <td>Priority</td>
      <td>Filter</td>
    </tr>
    <tr>
      <td>
        <select name="type">
          <option value="">ALL</option>
          <option <?php if ($_GET["type"] == "Alarm") echo " selected " ?>
            value="Alarm">Alarm</option>
          <option <?php if ($_GET["type"] == "Metric") echo " selected " ?>
            value="Metric">Metric</option>
        </select>
      </td>
      <td><input type="text" name="title" 
                 value="<?php echo $_GET["title"] ?>" /></td>
      <td><input type="text" name="in_charge" 
                 value="<?php echo $_GET["in_charge"] ?>" /></td>
      <td>
        <select name="status">
          <option value="">ALL</option>
          <option <?php if ($_GET["status"] == "Open") echo " selected " ?>
            value="Open">Open</option>
          <option <?php if ($_GET["status"] == "Closed") echo " selected " ?>
            value="Closed">Closed</option>
        </select>
      </td>
      <td>
        <select name="priority">
          <option value="">ALL</option>
          <option <?php if ($_GET["priority"] == "High") echo " selected " ?>
            value="High">High</option>
          <option <?php if ($_GET["priority"] == "Medium") echo " selected " ?>
            value="Medium">Medium</option>
          <option <?php if ($_GET["priority"] == "Low") echo " selected " ?>
            value="Low">Low</option>
        </select>
      </td>
      <td>
        <input type="submit" name="filter" value="OK" />
      </td>
    </tr>
  </table>
  </form>
  <br/>
  <!-- end filter -->

  <table align="center" width="100%">
<?php
    if ($incident_list = Incident::get_list($conn, "$where ORDER BY $order")) {
?>

    <tr>
      <?php 
        $filter = "&type=" . $_GET["type"] .
                  "&title=" . $_GET["title"] .
                  "&in_charge=" . $_GET["in_charge"] .
                  "&status=" . $_GET["status"] .
                  "&priority=" . $_GET["priority"]
      ?>
      <th><a href="<?php echo $_SERVER["PHP_SELF"] . 
            "?order=" . ossim_db::get_order("id", $order) . $filter ?>"
          >Ticket</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"] . 
            "?order=" . ossim_db::get_order("date", $order) . $filter ?>"
          >Date</a></th>
      <th>Last Modification</th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"] . 
            "?order=" . ossim_db::get_order("title", $order) . $filter ?>"
          >Title</a></th>
      <th>In Charge</th>
      <th>Status</th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"] . 
            "?order=" . ossim_db::get_order("priority", $order) . $filter ?>"
          >Priority</a></th>
    </tr>

<?php
        foreach ($incident_list as $incident) 
        {
            /* filter */
            if ($_GET["in_charge"]) 
            {
                if (false === stristr($incident->get_in_charge($conn),
                                      $_GET["in_charge"]))
                    continue;
            }

            if ($_GET["status"])
            {
                if ($incident->get_status($conn) != $_GET["status"])
                    continue;
            }

            if ($_GET["priority"])
            {
                $p = $incident->get_priority($conn);
                if ($_GET["priority"] == "High") {
                    if ($p < 7) continue;
                }
                elseif ($_GET["priority"] == "Medium") {
                    if (($p < 4) or ($p > 6)) continue;
                }
                elseif ($_GET["priority"] == "Low") {
                    if ($p > 3) continue;
                }
            }
?>

    <tr>
      <td><?php echo $incident->get_ticket(); ?></td>
      <td><?php echo $incident->get_date(); ?></td>
      <td><?php echo $incident->get_last_modification($conn); ?></td>
      <td><b><a href="incident.php?id=<?php echo $incident->get_id() ?>"><?php
        echo $incident->get_title(); ?></a></b>
      </td>
      <td><?php echo $incident->get_in_charge($conn); ?></td>
      <td>
        <?php 
            $status = $incident->get_status($conn);
            Incident::colorize_status($status);
        ?>
      </td>
      <?php 
        $priority = $incident->get_priority($conn);
        Incident::print_td_priority(
            $priority,
            Incident::get_priority_bgcolor($priority),
            Incident::get_priority_fgcolor($priority));
      ?>
    </tr>

<?php
        } /* foreach */
    } /* incident_list */
    else {
        echo "<p align=\"center\">There are no incidents on the system</p>";
    }

    $db->close($conn);
?>
    <tr>
      <td colspan="7" align="center">
        <a href="incident.php?insert=1&ref=Alarm&title=&priority=1&src_ips=&src_ports=&dst_ips=&dst_ports=">Insert new Incident</a>
      </td>
    </tr>
  </table>

</body>
</html>

