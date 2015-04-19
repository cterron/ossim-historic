<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuIncidents", "IncidentsTypes");
?>

<?php
    require_once ("ossim_db.inc");
    require_once ('classes/Incident_type.inc');
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Add new incident type"); ?> </h1>

<form method="post" action="newincidenttype.php">
<table align="center">
  <input type="hidden" name="insert" value="insert" />
  <tr>
    <th> <?php echo gettext("Type id"); ?> </th>
    <td class="left"><input type="text" id="type_id" name="id"  size="30" /></td>
  </tr>
  <tr>
    <th> <?php echo gettext("Description"); ?> </th>
    <td class="left">
      <textarea id="type_descr" name="descr"></textarea>
    </td>
  </tr>
  <tr>
    <td colspan="2" align="center" valign="top">
      <input type="submit" value="OK">
      <input type="reset" value="reset">
    </td>
  </tr>
</table>
</form>

</body>
</html>

