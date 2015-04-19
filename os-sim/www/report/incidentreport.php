<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuIncidents", "IncidentsReport");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Incident report"); ?> </h1>

<?php
    require_once ('ossim_db.inc');
    require_once ('classes/Incident.inc');

    $db = new ossim_db();
    $conn = $db->connect();

    incidents_by_status_table($conn);
    incidents_by_type_table($conn);
    incidents_by_user_table($conn);

    function incidents_by_status_table($conn)
    {
?>
    <h2 align="center"><?php echo gettext("Incidents by status"); ?></h2>
    <table align="center">
      <tr>
        <th><?php echo gettext("Incident Status") ?></th>
        <th><?php echo gettext("Ocurrences") ?></th>
      </tr>
<?php
        if ($list = Incident::incidents_by_status($conn))
        {
            foreach ($list as $l)
            {
                $status = $l[0];
                $occurrences = $l[1];
?>
      <tr>
        <td><?php Incident::colorize_status($status) ?></td>
        <td><?php echo $occurrences ?></td>
      </tr>
<?php
            }
        }
        
?>
      <tr>
        <td colspan="2">
          <img src="graphs/incidents_pie_graph.php?by=status"
               alt="incidents by status graph"/>
        </td>
      </tr>
    </table>
    <br/>
<?php
    }

    function incidents_by_type_table($conn)
    {
?>
    <h2 align="center"><?php echo gettext("Incidents by type"); ?></h2>
    <table align="center">
      <tr>
        <th><?php echo gettext("Incident type") ?></th>
        <th><?php echo gettext("Ocurrences") ?></th>
      </tr>
<?php
        if ($list = Incident::incidents_by_type($conn))
        {
            foreach ($list as $l)
            {
                $type = $l[0];
                $occurrences = $l[1];
?>
      <tr>
        <td><?php echo $type ?></td>
        <td><?php echo $occurrences ?></td>
      </tr>
<?php
            }
        }
        
?>
      <tr>
        <td colspan="2">
          <img src="graphs/incidents_pie_graph.php?by=type"
               alt="incidents by type graph"/>
        </td>
      </tr>
    </table>
    <br/>
<?php
    }

    function incidents_by_user_table($conn)
    {
?>
    <h2 align="center"><?php echo gettext("Incidents by user in charge"); ?></h2>
    <table align="center">
      <tr>
        <th><?php echo gettext("User in charge") ?></th>
        <th><?php echo gettext("Ocurrences") ?></th>
      </tr>
<?php
        if ($list = Incident::incidents_by_user($conn))
        {
            foreach ($list as $l)
            {
                $user = $l[0];
                $occurrences = $l[1];
?>
      <tr>
        <td><?php echo $user ?></td>
        <td><?php echo $occurrences ?></td>
      </tr>
<?php
            }
        }
        
?>
      <tr>
        <td colspan="2">
          <img src="graphs/incidents_pie_graph.php?by=user"
               alt="incidents by user graph"/>
        </td>
      </tr>
    </table>
    <br/>
<?php
    }

?>
<br/>

<h2 align="center"><?=_("Closed Incidents By Month")?></h2>
<p align="center">
<img src="graphs/incidents_bar_graph.php?by=monthly_by_status"
   alt="Num incidents closed by month"/></p>
<br/>

<h2 align="center"><?=_("Incident Resolution Time")?></h2>
<p align="center">
<img src="graphs/incidents_bar_graph.php?by=resolution_time"
   alt="incidents by resolution time"/></p>

</body>
</html>
