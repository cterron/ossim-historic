<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuReports", "ReportsIncidents");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
<style>
div.hidden{
  display: none;
}
div.error{
  display: inline;
  color: black;
  background-color: pink;
}
</style>
<script>
<?php
$proto = "http";
if ($_SERVER[HTTPS] == "on")
    $proto = "https";
require_once("ossim_conf.inc");
$ossim_conf = new ossim_conf();
$ossim_link = $ossim_conf->get_conf("ossim_link");
$url_check = "'$proto://$_SERVER[SERVER_ADDR]:$_SERVER[SERVER_PORT]/$ossim_link/incidents/check.php?q='";

?>


function checkName(input, response)
{
    if(input != ''){
    url  = <?php echo $url_check; ?> + input;
    loadXMLDoc(url);
    }
}

function rellena(valor)
{
      formulario = document.getElementById(11);
      formulario.value = valor;
}

function processReqChange() 
{
    // only if req shows "complete"
    if (req.readyState == 4) {
        // only if "OK"
        if (req.status == 200) {
            // ...processing statements go here...

      response  = req.responseXML.documentElement;

      method    = response.getElementsByTagName('method')[0].firstChild.data;

      message   = document.getElementById(10);
      formulario = document.getElementById(11);
      cadena = formulario.value;

      if(method != 0){
        message.className = 'error';
        message.innerHTML = '<h3><ul>';
        for(var i=0; i < method; i++){
        result    = response.getElementsByTagName('result')[i].firstChild.data;
        comienzo = result.indexOf(cadena) - 10;
        final2 = comienzo + cadena.length + 10;
        if(comienzo < 0) {
        message.innerHTML += '<li><a href="javascript:rellena(\'' + result + '\')"><b>' + result + '</b> ...</a>';
        } else {
        message.innerHTML += '<li><a href="javascript:rellena(\'' + result + '\')">... <b>' + result + '</b> ...</a>';
        }
        }
        message.innerHTML += '</ul>';
      } else {
        message.className = 'hidden';
      }

        } else {
            alert("There was a problem retrieving the XML data:\n" + req.statusText);
        }
    }
}

var req;

function loadXMLDoc(url) 
{
    // branch for native XMLHttpRequest object
    if (window.XMLHttpRequest) {
        req = new XMLHttpRequest();
        req.onreadystatechange = processReqChange;
        req.open("GET", url, true);
        req.send(null);
    // branch for IE/Windows ActiveX version
    } else if (window.ActiveXObject) {
        req = new ActiveXObject("Microsoft.XMLHTTP");
        if (req) {
            req.onreadystatechange = processReqChange;
            req.open("GET", url, true);
            req.send();
        }
    }
}
</script>
</head>
<body>

  <!-- <h1>Incidents</h1> -->

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Incident.inc';
    require_once 'classes/Incident_ticket.inc';

    /* admin privileges needed */
    function admin_error()
    {
        echo "
        <p align=\"center\"><font color=\"red\">
          Sorry, only admin user can do that.
        </font></p>
        ";
    }

    /* check email address */
    function check_copy_email($copy)
    {
        if ($copy) {
            if (!ereg("^[^@]+@[^@]+\.[^@]+$", $copy))
            {
                echo "
                <p align=\"center\"><font color=\"red\">
                  Copy mail address mal-formed
                </font></p>
                ";
                return False;
            }
        }
        return True;
    }

    /* copy to: */
    function send_action_mail($email, $descr, $action)
    {
        $msg = "
            $title
            
            $action
        ";

        if (mail($email, $descr, $msg) === false)
        {            
            echo "
            <p align=\"center\"><font color=\"red\">
              Error sending copy.
            </font></p>
            ";
        }
    }

    $db = new ossim_db();
    $conn = $db->connect();


    /* insert new incident and get its id */
    if ($_GET["insert"])
    {
        
        /* insert new alarm incident */
        if ($_GET["ref"] == 'Alarm')
        {
            if (!isset($_GET["title"]) or !isset($_GET["priority"]) or
                !isset($_GET["src_ips"]) or !isset($_GET["src_ports"]) or
                !isset($_GET["dst_ips"]) or !isset($_GET["dst_ports"]))
            {
                echo "<p align=\"center\">";
                printf(gettext("Error trying to insert new alarm ticket (argument missing)"));
                echo "</p>";
                exit;
            } else {
                $incident_id = Incident::insert_alarm (
                                    $conn, $_GET["title"], $_GET["priority"],
                                    $_GET["src_ips"], $_GET["dst_ips"],
                                    $_GET["src_ports"], $_GET["dst_ports"]);
            }
        } 

        /* insert new metric incident */
        elseif ($_GET["ref"] == 'Metric')
        {
            if (!isset($_GET["title"]) or !isset($_GET["priority"]) or
                !isset($_GET["target"]) or !isset($_GET["metric_type"]) or 
                !isset($_GET["metric_value"]))
            {
                echo "
                <p align=\"center\">
                  Error trying to insert new metric ticket (argument missing)
                </p>
                ";
                exit;
            } else {
                $incident_id = Incident::insert_metric (
                    $conn, $_GET["title"], $_GET["priority"],
                    $_GET["target"], $_GET["metric_type"], 
                    $_GET["metric_value"]);
            }

        } elseif ($_GET["ref"] == 'Hardware') { 
            if (!isset($_GET["title"]) or !isset($_GET["priority"]))
            {
                echo "<p align=\"center\">";
                printf(gettext("Error trying to insert new hardware ticket (argument missing)"));
                echo "</p>";
                exit;
            } else {
                $incident_id = Incident::insert_hardware (
                                $conn, $_GET["title"], $_GET["priority"]);
            }

        } elseif ($_GET["ref"] == 'Install') { 
            if (!isset($_GET["title"]) or !isset($_GET["priority"]))
            {
                echo "<p align=\"center\">";
                printf(gettext("Error trying to insert new install ticket (argument missing)"));
                echo "</p>";
                exit;
            } else {
                $incident_id = Incident::insert_install (
                                $conn, $_GET["title"], $_GET["priority"]);
            }
        }
    }
    
    /* get incident id to show */
    elseif (!$incident_id = $_GET["id"]) {
        echo "<b>Unknown incident</b>";
        exit;
    }

    $incident_list = Incident::get_list($conn, 
                                        "WHERE incident.id = $incident_id");
    $incident = $incident_list[0];

    /* insert new ticket */
    if ($_POST["insert_ticket"]) 
    {
        $valid_copy = check_copy_email($_POST["copy"]);

        /* read attachment */
        if (isset($_FILES["file"]))
        {
            $file = $_FILES["file"]["tmp_name"];

            $attachment = array (
                "name"    => $_FILES["file"]["name"],
                "size"    => $_FILES["file"]["size"],
                "type"    => $_FILES["file"]["type"],
                "content" => ""
            );

            
            if ($file)
            {
                $fd = fopen($file, "rb");
                $attachment["content"] = fread($fd, $attachment["size"]);
                $attachment["content"] = addslashes($attachment["content"]);
                fclose($fd);
            }
        }
        
        if ($_POST["description"] and $_POST["action"])
        {
            if ($valid_copy)
            {
                Incident_ticket::insert($conn,
                                        $incident_id,
                                        $_POST["status"],
                                        $_POST["priority"],
                                        Session::get_session_user(),
                                        $_POST["description"],
                                        $_POST["action"],
                                        $_POST["in_charge"],
                                        $_POST["transferred"],
                                        $_POST["copy"],
                                        $attachment);
                if ($_POST["copy"])
                    send_action_mail($_POST["copy"], 
                                     $_POST["description"],
                                     $_POST["action"]);
            }
        } else {
            echo "
            <p align=\"center\"><font color=\"red\">
              " . gettext("Description and Action fields are required") . "
            </font></p>
            ";
        }
    } 
    
    /* delete ticket (admin) */
    elseif ($_POST["submit_delete_ticket"])
    {
        if (Session::am_i_admin())
            Incident_ticket::delete($conn, $_POST["ticket_id"], $incident_id);
        else
            admin_error();
    } 
    
    /* increase priority (admin) */
    elseif ($_POST["submit_increase_priority"])
    {
        if (Session::am_i_admin())
        {
            $priority = $_POST["priority"];
            $priority = ($priority < 10)? $priority + 1 : $priority;
            Incident_ticket::update_priority (
                $conn, $_POST["ticket_id"], $incident_id, $priority);
        } else {
            admin_error();
        }
    }

    /* decrease priority (admin) */
    elseif ($_POST["submit_decrease_priority"])
    {
        if (Session::am_i_admin())
        {
            $priority = $_POST["priority"];
            $priority = ($priority > 0)? $priority - 1 : $priority;
            Incident_ticket::update_priority (
                $conn, $_POST["ticket_id"], $incident_id, $priority);
        } else {
            admin_error();
        }
    }

    /* open/close ticket (admin) */
    elseif ($_POST["submit_change_status"])
    {
        if (Session::am_i_admin())
            Incident_ticket::change_status($conn, $_POST["ticket_id"]);
        else
            admin_error();
    } 
    
    /* delete ticket (admin) */
    elseif ($_POST["submit_delete"])
    {
        if (Session::am_i_admin()) {
            Incident::delete($conn, $incident_id);
            echo "<p align=\"center\">Incident succesfully deleted</p>";
            echo "<p align=\"center\"><a href=\"index.php\">Back</a></p>";
            exit();
        } else {
            admin_error();
        }
    }

    /* edit incident */
    elseif ($_POST["submit_edit"])
    {
?>
        <form method="POST" 
              action="<?php echo $_SERVER["PHP_SELF"] . "?id=$incident_id" ?>">
        <table align="center">
          <tr>
            <th> <?php echo gettext("Name"); ?> </th>
            <td><input type="text" size="50" name="title"
                       value="<?php echo $incident->get_title(); ?>" />
            </td>
          </tr>
<?php
        if ($incident->get_ref() == "Alarm")
        {
            $alarms_list = $incident->get_alarms($conn);
            $alarms = $alarms_list[0];
?>
          <tr>
            <th> <?php echo gettext("Source"); ?> </th>
            <td>
              <input type="" size="50" name="src_ips"
                     value="<?php echo $alarms->get_src_ips() ?>" />
            </td>
          </tr>
          <tr>
            <th> <?php echo gettext("Destination"); ?> </th>
            <td>
              <input type="" size="50" name="dst_ips"
                     value="<?php echo $alarms->get_dst_ips() ?>" />
            </td>
          </tr>
          <tr>
            <th> <?php echo gettext("Src Ports"); ?> </th>
            <td>
              <input type="" size="50" name="src_ports"
                     value="<?php echo $alarms->get_src_ports() ?>" />
            </td>
          </tr>
          <tr>
            <th> <?php echo gettext("Dst Ports"); ?> </th>
            <td>
              <input type="" size="50" name="dst_ports"
                     value="<?php echo $alarms->get_dst_ports() ?>" />
            </td>
          </tr>
<?php
        } 
        
        elseif ($incident->get_ref() == "Metric") 
        {
            $metrics_list = $incident->get_metrics($conn);
            $metrics = $metrics_list[0];
?>
          <tr>
            <th> <?php echo gettext("Target"); ?> </th>
            <td>
              <input type="text" size="50" name="target"
                     value="<?php echo $metrics->get_target() ?>" />
            </td>
          </tr>
          <tr>
            <th> <?php echo gettext("Metric Type"); ?> </th>
            <td>
              <input type="text" size="50" name="metric_type"
                     value="<?php echo $metrics->get_metric_type() ?>" />
            </td>
          </tr>
          <tr>
            <th> <?php echo gettext("Metric Value"); ?> </th>
            <td>
              <input type="text" size="50" name="metric_value"
                     value="<?php echo $metrics->get_metric_value() ?>" />
            </td>
          </tr>
<?php
        }
?>
          <tr>
            <td colspan="2">
              <input type="submit" name="submit_edit_confirm" value="Edit" />
            </td>
          </tr>
        </table>
        </form>
<?php
        exit;
    }


    elseif ($_POST["submit_edit_confirm"])
    {
        if ($incident->get_ref() == "Alarm")
            Incident::update_alarm ($conn, 
                                    $incident_id,
                                    $_POST["title"],
                                    $_POST["src_ips"],
                                    $_POST["dst_ips"],
                                    $_POST["src_ports"],
                                    $_POST["dst_ports"]);

        elseif ($incident->get_ref() == "Metric")
            Incident::update_metric ($conn,
                                     $incident_id,
                                     $_POST["title"],
                                     $_POST["target"],
                                     $_POST["metric_type"],
                                     $_POST["metric_value"]);

        elseif ($incident->get_ref() == "Hardware")
            Incident::update_hardware ($conn,
                                       $incident_id,
                                       $_POST["title"]);

        elseif ($incident->get_ref() == "Install")
            Incident::update_install ($conn,
                                      $incident_id,
                                      $_POST["title"]);
            

        /* re-read from db */
        $incident_list = Incident::get_list($conn, 
                                            "WHERE incident.id = $incident_id");
        $incident = $incident_list[0];
    }

    if ($incident) {
?>


<!-- incident summary -->
<form method="post" action="<?php echo $_SERVER["PHP_SELF"] .
      "?id=" . $incident->get_id(); ?>">
<table align="center" width="100%">
  <tr>
    <th> <?php echo gettext("Ticket"); ?> </th>
    <th width="550px">
    <?php echo gettext("Incident"); ?> </th>
    <th> <?php echo gettext("In Charge"); ?> </th>
    <th> <?php echo gettext("Status"); ?> </th>
    <th> <?php echo gettext("Priority"); ?> </th>
    <th> <?php echo gettext("Action"); ?> </th>
  </tr>
  <tr>
    <td><b><?php echo $incident->get_ticket() ?></b></td>

    <!-- incident data -->
    <td class="left">
    <?php
        $title = $incident->get_title();
        $ref = $incident->get_ref();
        echo "
          Name: <b>$title</b><br/>
          Type: $ref<br/><hr/>
        ";
        if ($ref == 'Alarm')
        {
            if ($alarm_list = $incident->get_alarms($conn))
            {
                foreach ($alarm_list as $alarm_data)
                {
                    echo 
                        "Source Ips: <b>" . 
                            $alarm_data->get_src_ips() . "</b> - " .
                        "Dest Ips: <b>" . 
                            $alarm_data->get_dst_ips() . "</b> - " .
                        "Source Ports: <b>" . 
                            $alarm_data->get_src_ports() . "</b> - " .
                        "Dest Ports: <b>" .
                            $alarm_data->get_dst_ports() . "</b>";
                }
            }
        }
        elseif ($ref == 'Metric')
        {
            if ($metric_list = $incident->get_metrics($conn))
            {
                foreach ($metric_list as $metric_data)
                {
                    echo 
                        "Target: <b>" .
                            $metric_data->get_target() . "</b> - " .
                        "Metric Type: <b>" . 
                            $metric_data->get_metric_type() . "</b> - " .
                        "Metric Value: <b>" . 
                            $metric_data->get_metric_value() . "</b>";
                }
            }
        }
    ?>
    </td>
    <!-- end incident data -->

    <td><?php echo $incident->get_in_charge($conn) ?></td>
    <td><?php Incident::colorize_status($incident->get_status($conn)) ?></td>

    <!-- priority -->
    <?php
        $priority = $incident->get_priority($conn);
        Incident::print_td_priority(
            $priority,
            Incident::get_priority_bgcolor($priority),
            Incident::get_priority_fgcolor($priority));
    ?>
    <!-- end priority -->

    <td>
        <input type="submit" name="submit_edit" value="Edit"
               style="width: 10em;" /><br/>
        <input type="submit" name="submit_delete" value="Delete"
               style="width: 10em; color: red; text-decoration: bold" />
    </td>
  </tr>
</table>
</form>
<!-- end incident summary -->


<!-- list of tickets tickets -->
<br/><br/>
<table align="center" width="100%">
  <tr>
    <th> <?php echo gettext("Date"); ?> </th>
    <th> <?php echo gettext("User / Description / Action"); ?> </th>
    <th> <?php echo gettext("Priority"); ?> </th>
    <th> <?php echo gettext("Status"); ?> </th>
    <th> <?php echo gettext("In Charge"); ?> </th>
    <th> <?php echo gettext("Transferred"); ?> </th>
    <th> <?php echo gettext("Copy"); ?> </th>
    <th> <?php echo gettext("Action"); ?> </th>
  </tr>

<?php
    if ($incident_tickets = $incident->get_tickets($conn))
    {
        foreach ($incident_tickets as $incident_ticket)
        {
?>
  <tr>
    <form method="post" action="<?php echo $_SERVER["PHP_SELF"] .
      "?id=" . $incident->get_id(); ?>">
    <input type="hidden" name="ticket_id"
           value="<?php echo $incident_ticket->get_id(); ?>" />
    <td><?php echo $incident_ticket->get_date(); ?></td>
    <td class="left">
      <b> <?php echo gettext("User"); ?> </b>:
      <?php echo $incident_ticket->get_user(); ?><br/><hr/>
      <b> <?php echo gettext("Description"); ?> </b>:
      <?php echo $incident_ticket->get_description(); ?><br/><hr/>
      <b> <?php echo gettext("Action"); ?> </b>:
      <?php echo $incident_ticket->get_action(); ?>

<?php
    /* attachment */
    if ($attachment = $incident_ticket->get_attachment($conn))
    {
        $file_id      = $attachment->get_id();
        $file_name    = $attachment->get_name();
        $file_content = $attachment->get_content();
        $file_type    = $attachment->get_type();
        
        echo "<br/><hr/><b>Attachment</b>:&nbsp;";
        echo "<a href=\"attachment.php?id=$file_id\">" . 
            $attachment->get_name() . "</a>";
    }
?>

    </td>
    <?php 
        // NOTE: priority is used in insert form
        $priority = $incident_ticket->get_priority($conn);
        Incident::print_td_priority(
            $priority,
            Incident::get_priority_bgcolor($priority),
            Incident::get_priority_fgcolor($priority));
    ?>
      <input type="hidden" name="priority" value="<?php echo $priority ?>">
    <td>
    <?php
        // NOTE: status and status_action are used in insert form
        $status = $incident_ticket->get_status();
        if ($status == 'Open')
            $status_action = 'Close';
        elseif ($status == 'Closed')
            $status_action = 'Open';
        Incident::colorize_status($status);
    ?>
    </td>
    <td>
        <?php
            // NOTE: in_charge is used in insert form
            $in_charge = $incident_ticket->get_in_charge();
            echo $in_charge; 
        ?>
    </td>
    <td>
        <?php
            $transferred = $incident_ticket->get_transferred();
            if ($transferred)
                echo $transferred;
            else
                echo "-";
        ?>
    </td>
    <td>
        <?php
            $copy = $incident_ticket->get_copy();
            if ($copy)
                echo $copy;
            else
                echo "-";
        ?>
    </td>
    <td nowrap>
      <input type="submit" name="submit_increase_priority" 
             value="Increase Priority" style="width: 10em;" /><br/>
      <input type="submit" name="submit_decrease_priority" 
             value="Decrease Priority" style="width: 10em;" /><br/>
<?php
        if (isset($status_action)) {
?>
      <input type="submit" name="submit_change_status" 
             value="<?php echo $status_action ?>" 
             style="width: 10em; color: #17457c" /><br/>
<?php
        }
?>
      <input type="submit" name="submit_delete_ticket" 
             value="Delete" style="width: 10em; color: red;" />
    </td>
    </form>
  </tr>
<?php
        }
    }
?>
  <tr><td colspan="8"></td></tr>
  <tr><td colspan="8"></td></tr>
  <form enctype="multipart/form-data" method="post"
        action="<?php echo $_SERVER["PHP_SELF"] .
        "?id=" . $incident->get_id(); ?>">
  <tr>
    <td><?php echo strftime("%A %d-%b-%Y", time()) ?></td>
    <td>
       <?php echo gettext("Description"); ?> <br/>
      <textarea id="11" name="description" rows="6" autocomplete="off" onKeyUp="checkName(this.value,'')" cols="40"></textarea><br/>
       <?php echo gettext("Action"); ?> <br/>
      <textarea name="action" rows="6" cols="40"></textarea><br/>
       <?php echo gettext("Attachment"); ?> <br/>
      <input type="file" name="file" />
    </td>
    <td colspan="2">
      <?php echo gettext("Priority"); ?> <br/>
      <select name="priority">
        <option value="1" 
            <?php if ($priority == 1) echo "SELECTED" ?>>
	    <?php echo gettext("1"); ?> </option>
        <option value="2"
            <?php if ($priority == 2) echo "SELECTED" ?>>
	    <?php echo gettext("2"); ?> </option>
        <option value="3"
            <?php if ($priority == 3) echo "SELECTED" ?>>
	    <?php echo gettext("3"); ?> </option>
        <option value="4"
            <?php if ($priority == 4) echo "SELECTED" ?>>
	    <?php echo gettext("4"); ?> </option>
        <option value="5"
            <?php if ($priority == 5) echo "SELECTED" ?>>
	    <?php echo gettext("5"); ?> </option>
        <option value="6"
            <?php if ($priority == 6) echo "SELECTED" ?>>
	    <?php echo gettext("6"); ?> </option>
        <option value="7"
            <?php if ($priority == 7) echo "SELECTED" ?>>
	    <?php echo gettext("7"); ?> </option>
        <option value="8"
            <?php if ($priority == 8) echo "SELECTED" ?>>
	    <?php echo gettext("8"); ?> </option>
        <option value="9"
            <?php if ($priority == 9) echo "SELECTED" ?>>
	    <?php echo gettext("9"); ?> </option>
        <option value="10"
            <?php if ($priority == 10) echo "SELECTED" ?>>
	    <?php echo gettext("10"); ?> </option>
      </select><br/><br/>
      <?php echo gettext("status"); ?> <br/>
      <select name="status">
        <option value="Open"
            <?php if ($status == 'Open') echo "SELECTED" ?>>
	    <?php echo gettext("Open"); ?> </option>
        <option value="Closed"
            <?php if ($status == 'Closed') echo "SELECTED" ?>>
	    <?php echo gettext("Closed"); ?> </option>
      </select>
    </td>
    <td colspan="3">
      <?php echo gettext("In charge"); ?> :<br/>
      <b>
        <?php
            if ($transferred) 
                $in_charge = $transferred;
            if (!$in_charge) // first action
                $in_charge = Session::get_session_user();

            echo $in_charge; 
        ?>
      </b>
      <br/><br/>
      <input type="hidden" name="in_charge" 
             value="<?php echo $in_charge ?>" />
      <?php echo gettext("Transfer"); ?> <br/>
      <input type="text" name="transferred" /><br/>
      <?php echo gettext("Copy (e-mail)"); ?> <br/>
      <input type="text" name="copy" />
    </td>
    <td><input type="submit" name="insert_ticket" value="Add" /></td>
  </tr>
  </form>
</table>
<!-- end list of incidents -->
<div id="10">
</div>

<?php
    } /* if incident */
    $db->close($conn);
?>

</body>
</html>

