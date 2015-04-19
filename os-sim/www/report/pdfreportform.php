<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuReports", "ReportsPDFReport");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("PDF reports"); ?> </h1>

<?php
    require_once 'classes/Security.inc';

    $report_type = GET('report_type');

    ossim_valid($report_type, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("Report Type"));

    if (ossim_error()) {
            die(ossim_error());
    }
    
    if (empty($report_type)) $report_type = "security";

    require_once('ossim_conf.inc');
    $path_conf = $GLOBALS["CONF"];
    $fpdf_path = $path_conf->get_conf("fpdf_path");

    if (!is_readable($fpdf_path)) {
            $error = new OssimError();
            $error->display("FPDF_PATH");
    }
?>

  <!-- report selector -->
  <form name="report_selector" method="GET">
  <table align="center">
    <tr><td>
    <select name="report_type" 
        onChange="document.forms['report_selector'].submit()">
      <option 
        <?php if ($report_type == "security") echo " selected "; ?>
        value="security"><?php echo gettext('Security Report'); ?></option>
      <option 
        <?php if ($report_type == "alarms") echo " selected "; ?>
        value="alarms"><?php echo gettext('Alarms Report'); ?></option>
      <option 
        <?php if ($report_type == "metrics") echo " selected "; ?>
        value="metrics"><?php echo gettext('Metrics Report'); ?></option>
      <option 
        <?php if ($report_type == "incident") echo " selected "; ?>
        value="incident"><?php echo gettext('Incidents Report'); ?></option>
    </select>
    </td></tr>
  </table>
  </form>
  <!-- end report selector -->


  <table align="center">
  <form action="pdfreport.php" method="POST" />
<?php
    if ($report_type == "security") {
        security_report();
    } elseif ($report_type == "metrics") {
        metrics_report();
    } elseif ($report_type == "incident") {
        incident_report();
    } elseif ($report_type == "alarms") {
        alarms_report();
    }
    
?>
  </form>
  </table>

<?php

    function security_report()
    {
?>
    <tr>
      <th> <?php echo gettext("Security Report Options"); ?> </th>
    </tr>
    <tr>
      <!-- security report options -->
      <td>
        <table align="center" valign="center">
          <tr>
            <td class="left">
              <input type="checkbox" name="attacked" checked>
                <?php echo gettext("Top Attacked Hosts"); ?> 
              </input>
            </td>
          </tr>
          <tr>
            <td class="left">
              <input type="checkbox" name="attacker" checked>
                <?php echo gettext("Top AttackerHosts"); ?> 
              </input>
            </td>
          </tr>
          <tr>
            <td class="left">
              <input type="checkbox" name="ports" checked>
                <?php echo gettext("Top Destination Ports"); ?> 
              </input>
            </td>
          </tr>
          <tr>
            <td class="left">
              <input type="checkbox" name="eventsbyhost" checked>
                <?php echo gettext("Top Events by Host"); ?> 
              </input>
            </td>
          </tr>
          <tr>
            <td class="left">
              <input type="checkbox" name="eventsbyrisk" checked>
                <?php echo gettext("Top Events by Risk"); ?> 
              </input>
            </td>
          </tr>
          <tr>
            <td>
              <?php echo gettext("Number of hosts per table"); ?> : 
              <input type="text" size="2" name="limit" value="15" />
            </td>
          </tr>
        </table>
      </td>
      <!-- end security report options -->
    </tr>
    <tr>
      <td><input type="submit" name="submit_security" value="<?php echo gettext('Generate'); ?>" /></td>
    </tr>
<?php
    }

    function metrics_report()
    {
?>
    <tr>
      <th> <?php echo gettext("Metrics Report Options"); ?> </th>
    </tr>
    <tr>
      <!-- metrics report -->
      <td>
        <table align="center" valign="center">
          <tr>
            <td class="left">
              <input type="checkbox" checked name="time_day"> 
	          <?php echo gettext("Day"); ?> </input>
            </td>
          </tr>
          <tr>
            <td class="left">
              <input type="checkbox" checked name="time_week">
	          <?php echo gettext("Week"); ?> </input>
            </td>
          </tr>
          <tr>
            <td class="left">
              <input type="checkbox" checked name="time_month">
	          <?php echo gettext("Month"); ?> </input>
            </td>
          </tr>
          <tr>
            <td class="left">
              <input type="checkbox" name="time_year">
	          <?php echo gettext("Year"); ?> </input>
            </td>
          </tr>
        </table>
      </td>
      <!-- end metrics report -->
    </tr>
    <tr>
      <td><input type="submit" name="submit_metrics" value="<?php echo gettext('Generate'); ?>" /></td>
    </tr>
<?php
    }

    function incident_report()
    {
?>
    <tr>
      <th> <?php echo gettext("Incident Report Options"); ?> </th>
    </tr>
    <tr>
      <!-- incident report -->
      <td>
        <table align="center" valign="center">
          <tr>
            <td><?php echo gettext("Reason: ") ?></td>
            <td><textarea name="reason"></textarea>
          </tr>
          <tr>
            <td><?php echo gettext("Date: ") ?></td>
            <td><input type="text" size="22" name="date"
                       value="<?php echo date("F j, Y, g:i a"); ?>" />
            </td>
          </tr>
          <tr>
            <td><?php echo gettext("Location: ") ?></td>
            <td><input type="text" name="location" size="22" /></td>
          </tr>
          <tr>
            <td><?php echo gettext("In Charge: ") ?></td>
            <td><input type="text" name="in_charge" size="22" /></td>
          </tr>
          <tr>
            <td><?php echo gettext("Summary: ") ?></td>
            <td><textarea name="summary"></textarea>
          </tr>
          <tr>
            <td><?php echo gettext("Metrics notes: ") ?></td>
            <td><textarea name="metrics_notes"></textarea></td>
          </tr>
          <tr>
            <td><?php echo gettext("Alarms notes: ") ?></td>
            <td><textarea name="alarms_notes"></textarea></td>
          </tr>
          <tr>
            <td><?php echo gettext("Events notes: ") ?></td>
            <td><textarea name="events_notes"></textarea></td>
          </tr>
        </table>
      </td>
      <!-- end incident report -->
    </tr>
    <tr>
      <td><input type="submit" name="submit_incident" value="<?php echo gettext('Generate'); ?>" /></td>
    </tr>
<?php
    }

function alarms_report()
    {
?>
    <tr>
      <th> <?php echo gettext("Alarms Report Options"); ?> </th>
    </tr>
    <tr>
      <!-- alarms report options -->
      <td>
        <table align="center" valign="center">
          <tr>
            <td class="left">
              <input type="checkbox" name="attacked" checked>
                <?php echo gettext("Top Attacked Hosts"); ?>
              </input>
            </td>
          </tr>
          <tr>
            <td class="left">
              <input type="checkbox" name="attacker" checked>
                <?php echo gettext("Top Attacker Hosts"); ?>
              </input>
            </td>
          </tr>
          <tr>
            <td class="left">
              <input type="checkbox" name="ports" checked>
                <?php echo gettext("Top Destination Ports"); ?>
              </input>
            </td>
          </tr>
          <tr>
            <td class="left">
              <input type="checkbox" name="alarmsbyhost" checked>
                <?php echo gettext("Top Alarms by Host"); ?>
              </input>
            </td>
          </tr>
          <tr>
            <td class="left">
              <input type="checkbox" name="alarmsbyrisk" checked>
                <?php echo gettext("Top Alarms by Risk"); ?>
              </input>
            </td>
          </tr>
          <tr>
            <td>
              <?php echo gettext("Number of hosts per table"); ?> :
              <input type="text" size="2" name="limit" value="15" />
            </td>
          </tr>
        </table>
      </td>
      <!-- end security report options -->
    </tr>
    <tr>
      <td><input type="submit" name="submit_alarms" value="<?php echo gettext('Generate'); ?>" /></td>
    </tr>
<?php
    }
?>
</html>

