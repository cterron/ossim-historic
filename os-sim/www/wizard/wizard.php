<html>
<head>
  <title> OSSIM Wizard </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>

<body>

  <h1>OSSIM Framework</h1>
  <h2>Wizard</h2>


<?php
    if (!$step = $_POST["step"]) {
?>

  <table align="center">
    <tr><th>Welcome to the OS-SIM configuration wizard</th></tr>
    <tr><td>
      <p>This wizard will help you to configure OS-SIM for your machine.<br/>
      (Use predetermined values in case of doubt)</p>
      <p>If you have problems during the installation, tell us:<br/>
      <tt>os-sim-support@sourceforge.net</tt></p>
      <p>Please, press Next button to continue.</p>
    </td></tr>
    <tr><td>
      <form method="post" action="wizard.php">
        <input type="hidden" name="step" value="1">
        <input type="submit" value="Next =>">
      </form>
    </td></tr>
  </table>

<?php } elseif ($step == 1) { ?>

  <!-- STEP 1 -->
  <table align="center">
    <tr>
      <th colspan="2">Database Configuration</th>
    </tr>
    <tr>      
      <td><p>Enter the hostname of the mysql database server to use:</p></td>
      <td><input type="text" name="ossim_host" value="localhost"></td>
    </tr>
    <tr>
      <td><p>Enter the name of the database to use:</p></td>
      <td><input type="text" name="ossim_base" value="ossim"></td>
    </tr>
    <tr>
      <td><p>Enter the name of the database user you want to use:</p></td>
      <td><input type="text" name="ossim_user" value="root"></td>
    </tr>
    <tr>
      <td><p>Enter the password for the database connection:</p></td>
      <td><input type="password" name="ossim_pass"></td>
    </tr>

    <tr><td colspan="2">
        <form method="post" action="wizard.php">
          <input type="submit" value="<= Back">
        </form>
        <form method="post" action="wizard.php">
          <input type="hidden" name="step" value="2">
          <input type="submit" value="Next =>">
        </form>
    </td></tr>
  </table>
  <!-- end STEP 1 -->

<?php } elseif ($step == 2) { ?>

  <!-- STEP 2 -->
  <table align="center">
    <tr><th>Database Configuration</th></tr>
    <tr><td>
      <p>Please, create the database structure now, using the following
      command:</p>
            
      <p><table align="center"><tr><td>
        cd /PATH/TO/OS-SIM/db/ && ./create_ossim_tables.pl
      </td></tr></table></p>
      
      <p>After you created the database structure like this, press 'Next' to
      continue.</p>
    </td></tr>
    <tr><td>
        <form method="post" action="wizard.php">
          <input type="hidden" name="step" value="1">
          <input type="submit" value="<= Back">
        </form>
        <form method="post" action="wizard.php">
          <input type="hidden" name="step" value="3">
          <input type="submit" value="Next =>">
        </form>
    </td></tr>
  </table>
  <!-- end STEP 2 -->

<?php } elseif ($step == 3) { ?>

  <!-- STEP 3 -->
  <table align="center">
    <tr><th colspan="2">Snort</th></tr>
    <tr><td colspan="2">
      <p>OS-SIM use <b>Snort</b>, the Open Source Network Intrusion Detection
      System.</p>
    </td></tr>
    <tr>
      <td><p>Enter the path to snort:</p></td>
      <td><input type="text" name="snort_path" value="/etc/snort"></td>
    </tr>
    <tr>
      <td><p>Enter the path to the snort rules directory:</p></td>
      <td><input type="text" name="snort_rules_path" 
                 value="/etc/snort/rules"></td>
    </tr>
    <tr><td colspan="2">
        <form method="post" action="wizard.php">
          <input type="hidden" name="step" value="2">
          <input type="submit" value="<= Back">
        </form>
        <form method="post" action="wizard.php">
          <input type="hidden" name="step" value="4">
          <input type="submit" value="Next =>">
        </form>
    </td></tr>
  </table>
  <!-- end STEP 3 -->

<?php } elseif ($step == 4) { ?>

  <!-- STEP 4 -->
  <table align="center">
    <tr><th colspan="2">Snort</th></tr>
    <tr><td colspan="2">
      <p>You have to tell OS-SIM where the snort database is, and how to loggin
      in. <br/>(Read <tt>/etc/snort/snort.conf</tt> to get this info)</p>      
    </td></tr>
    
    <tr>      
      <td><p>Enter the hostname of the snort database server to use:</p></td>
      <td><input type="text" name="snort_host" value="localhost"></td>
    </tr>
    <tr>
      <td><p>Enter the name of the snort database to use:</p></td>
      <td><input type="text" name="snort_base" value="snort"></td>
    </tr>
    <tr>
      <td><p>Enter the name of the snort database user you want to use:</p></td>
      <td><input type="text" name="snort_user" value="snort"></td>
    </tr>
    <tr>
      <td><p>Enter the password for the snort database connection:</p></td>
      <td><input type="password" name="snort_pass"></td>
    </tr>
    <tr><td colspan="2">
        <form method="post" action="wizard.php">
          <input type="hidden" name="step" value="3">
          <input type="submit" value="<= Back">
        </form>
        <form method="post" action="wizard.php">
          <input type="hidden" name="step" value="5">
          <input type="submit" value="Next =>">
        </form>
    </td></tr>
  </table>
  <!-- end STEP 4 -->

<?php } elseif ($step == 5) { ?>

  <!-- STEP 5 -->
  <table align="center">
    <tr><th colspan="2">ADOdb</th></tr>
    <tr><td colspan="2">
      <p>ADOdb is a database abstraction layer for php that<br/> allows the
      generic access to any database.</p>
    </td></tr>
    <tr>
      <td><p>Enter the path to ADOdb:</p></td>
      <td><input type="text" name="adodb_path" value="/usr/share/adodb"></td>
    </tr>
    <tr><td colspan="2">
        <form method="post" action="wizard.php">
          <input type="hidden" name="step" value="4">
          <input type="submit" value="<= Back">
        </form>
        <form method="post" action="wizard.php">
          <input type="hidden" name="step" value="6">
          <input type="submit" value="Next =>">
        </form>
    </td></tr>
  </table>
  <!-- end STEP 5 -->

<?php } elseif ($step == 6) { ?>

  <!-- STEP 6 -->
  <table align="center">
    <tr><th colspan="2">RRDtool</th></tr>
    <tr><td colspan="2">
      <p>RRD is a system to store and display time-series data.</p>
    </td></tr>
    <tr>
      <td><p>Enter the path to RRDtool:</p></td>
      <td><input type="text" name="rrdtool_path"
                 value="/usr/local/rrdtool-1.1.0/bin/"></td>
    </tr>
    <tr>
      <td><p>Enter the path to RRDtool lib directory:</p></td>
      <td><input type="text" name="rrdtool_lib_path"
                 value="/usr/local/rrdtool-1.1.0/lib/perl/"></td>
    </tr>
    <tr><th colspan="2">MRTG</th></tr>
    <tr><td colspan="2">
      <p>MRTG generates HTML pages containing graphical images which<br/> 
         provide a visual representation of the traffic on network-links</p>
    </td></tr>
    <tr>
      <td><p>Enter the path to MRTG:</p></td>
      <td><input type="text" name="mrtg_path"
                 value="/usr/bin/"></td>
    </tr>
    <tr><td colspan="2">
        <form method="post" action="wizard.php">
          <input type="hidden" name="step" value="5">
          <input type="submit" value="<= Back">
        </form>
        <form method="post" action="wizard.php">
          <input type="hidden" name="step" value="7">
          <input type="submit" value="Next =>">
        </form>
    </td></tr>
  </table>
  <!-- end STEP 6 -->
  
<?php } ?>

</body>
</html>
