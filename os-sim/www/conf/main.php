<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationMain");
?>

<html>
<head>
  <title> Riskmeter </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1>Main Configuration</h1>
  
  <table align="center">
    <tr><th colspan="2">Reload server structures</th></tr>
    <tr><td colspan="2"></td></tr>
    <tr>
      <td colspan="2">
        <a href="reload.php">RELOAD ALL</a>
      </td>
    </tr>
    <tr>
      <td colspan="2">
        <a href="reload.php?what=policies">Reload policies</a>
      </td>
    </tr>
    <tr>
      <td colspan="2">
        <a href="reload.php?what=hosts">Reload hosts</a>
      </td>
    </tr>
    <tr>
      <td colspan="2">
        <a href="reload.php?what=nets">Reload nets</a>
      </td>
    </tr>
    <tr>
      <td colspan="2">
        <a href="reload.php?what=sensors">Reload sensors</a>
      </td>
    </tr>
    <tr>
      <td colspan="2">
        <a href="reload.php?what=directives">Reload directives</a>
      </td>
    </tr>
    <tr><td colspan="2"><hr noshade></td></tr>
    <tr><th colspan="2"><a href="../setup/ossim_acl.php">Reload ACLS</a></th></tr>
    <tr><td colspan="2"></td></tr>

  </table>
    
  
</body>
</html>
