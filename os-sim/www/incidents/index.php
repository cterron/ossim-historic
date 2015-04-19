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

    $type        = mysql_real_escape_string($_GET["type"]);
    $title       = mysql_real_escape_string($_GET["title"]);
    $user        = mysql_real_escape_string($_GET["user"]);
    $description = mysql_real_escape_string($_GET["description"]);
    $action      = mysql_real_escape_string($_GET["action"]);
    $attachment  = mysql_real_escape_string($_GET["attachment"]);
    $copyto      = mysql_real_escape_string($_GET["copyto"]);
    
    if ($_GET["type"])
        $where .= "incident.ref = '$type' ";
    if ($_GET["title"]) {
        if ($where) $where .= "AND ";
        $where .= "incident.title LIKE '%$title%'";
    }
    if ($_GET["user"]) {
        if ($where) $where .= "AND ";
        $where .= "incident_ticket.users LIKE '%$user%'";
    }
    if ($_GET["description"]) {
        if ($where) $where .= "AND ";
        $where .= "incident_ticket.description LIKE '%$description%'";
    }
    if ($_GET["action"]) {
        if ($where) $where .= "AND ";
        $where .= "incident_ticket.action LIKE '%$action%'";
    }
    if ($_GET["attachment"]) {
        if ($where) $where .= "AND ";
        $where .= "incident_file.name LIKE '%$attachment%'";
    }
    if ($_GET["copyto"]) {
        if ($where) $where .= "AND ";
        $where .= "incident_ticket.copy LIKE '%$copyto%'";
    }
    if ($where) $where = "WHERE " . $where;

?>

  <!-- filter -->
  <form method="GET" action="<?php echo $_SERVER["PHP_SELF"] ?>">
    <?php if (isset($_GET["advanced_search"])) { ?>
    <input type="hidden" name="advanced_search" 
           value="<?php echo $_GET["advanced_search"] ?>">
    <?php } ?>
  <table align="center">
    <tr>
      <th colspan="6">Filter
<?php

        if (isset($_GET["advanced_search"])) {
            echo "[<a href=\"" . 
                $_SERVER["PHP_SELF"] ."\"
                title=\"Click to change to simple filter\">Advanced</a>]";
        } else {
            echo "[<a href=\"" . 
                $_SERVER["PHP_SELF"] ."?advanced_search=1\"
                title=\"Click to change to advanced filter\">Simple</a>]";
        }
?>
      </th>
    </tr>
    <tr>
      <td>Type</td>
      <td>Title</td>
      <td>In charge</td>
      <td>Status</td>
      <td>Priority</td>
      <td>Action</td>
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
      <td nowrap>
        <input type="submit" name="filter" value="OK" />
      </td>
    </tr>
<?php
    if (isset($_GET["advanced_search"])) {
?>
    <tr>
      <td>with User</td>
      <td>with Description</td>
      <td>with Action</td>
      <td>with Attachment</td>
      <td>with Copy to</td>
      <td></td>
    </tr>
    <tr>
      <td><input type="text" name="user"
                 value="<?php echo $_GET["user"] ?>" /></td>
      <td><input type="text" name="description"
                 value="<?php echo $_GET["description"] ?>" /></td>
      <td><input type="text" name="action"
                 value="<?php echo $_GET["action"] ?>" /></td>
      <td><input type="text" name="attachment"
                 value="<?php echo $_GET["attachment"] ?>" /></td>
      <td><input type="text" name="copyto"
                 value="<?php echo $_GET["copyto"] ?>" /></td>
    </tr>
<?php
    }
?>
  </table>
  </form>
  <br/>
  <!-- end filter -->

  <table align="center" width="100%">
<?php
    if ($incident_list = Incident::get_list($conn, 
        "$where ORDER BY " . mysql_real_escape_string($order))) {

        $filter = "&type="        . $_GET["type"] .
                  "&title="       . $_GET["title"] .
                  "&in_charge="   . $_GET["in_charge"] .
                  "&status="      . $_GET["status"] .
                  "&priority="    . $_GET["priority"] .
                  "&user="        . $_GET["user"] .
                  "&description=" . $_GET["description"] .
                  "&action="      . $_GET["action"] .
                  "&attachment="  . $_GET["attachment"] .
                  "&copyto="      . $_GET["copyto"];
        if (isset($_GET["advanced_search"]))
            $filter .= $_GET["advanced_search"];
?>

    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"] . 
            "?order=" . ossim_db::get_order("id", $order) . "$filter" ?>"
          >Ticket</a></th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"] . 
            "?order=" . ossim_db::get_order("date", $order) ."$filter" ?>"
          >Date</a></th>
      <th>Last Modification</th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"] . 
            "?order=" . ossim_db::get_order("title", $order) . "$filter" ?>"
          >Title</a></th>
      <th>In Charge</th>
      <th>Status</th>
      <th><a href="<?php echo $_SERVER["PHP_SELF"] . 
            "?order=" . ossim_db::get_order("priority", $order) . "$filter" ?>"
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
                    if (($p < 5) or ($p > 6)) continue;
                }
                elseif ($_GET["priority"] == "Low") {
                    if ($p > 4) continue;
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
        echo "<p align=\"center\">No incidents</p>";
    }

    $db->close($conn);
?>
    <tr>
      <td colspan="7" align="center">
        Insert new Incident (
        <a href="incident.php?insert=1&ref=Alarm&title=new_incident&priority=1&src_ips=&src_ports=&dst_ips=&dst_ports=">Alarm</a> | 
        <a href="incident.php?insert=1&ref=Metric&title=Metric threshold&priority=1&target=&metric_type=&metric_value=">Metric</a>
        )
      </td>
    </tr>
  </table>

</body>
</html>

