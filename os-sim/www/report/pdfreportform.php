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

  <table align="center">
  <form action="pdfreport.php" method="POST" />
    <tr>
      <th> <?php echo gettext("Security Report"); ?> </th>
      <th> <?php echo gettext("Metrics Report"); ?> </th>
      <th> <?php echo gettext("Incident Report"); ?> </th>
      <!--
      <th>N-Day Report</th>
      -->
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
              <input type="checkbox" name="alertsbyhost" checked>
                <?php echo gettext("Top Alerts by Host"); ?> 
              </input>
            </td>
          </tr>
          <tr>
            <td class="left">
              <input type="checkbox" name="alertsbyrisk" checked>
                <?php echo gettext("Top Alerts by Risk"); ?> 
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

      <!-- incident report -->
      <td>
        <table align="center" valign="center">
          <tr>
            <td><input name="metrics" 
                type="checkbox" checked>
		<?php echo gettext("Metrics"); ?> </input></td>
          </tr>
          <tr>
            <td><input name="alarms"
                type="checkbox" checked>
		<?php echo gettext("Alarms"); ?> </input></td>
          </tr>
        </table>
      </td>
      <!-- end incident report -->

      
      <!--
      <td>Options<br/>Working on...</td>
      -->
    </tr>
    <tr>
      <td><input type="submit" name="submit_security" value="Generate" /></td>
      <td><input type="submit" name="submit_metrics" value="Generate" /></td>
      <td><input type="submit" name="submit_incident" value="Generate" /></td>
      <!--
      <td><input type="submit" name="" value="Generate" /></td>
      -->
    </tr>
  </form>
  </table>

</body>
</html>

