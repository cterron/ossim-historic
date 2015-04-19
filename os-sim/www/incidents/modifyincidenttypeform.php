<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuIncidents", "IncidentsTypes");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Modify Incident Type"); ?> </h1>

<?php
    require_once 'classes/Security.inc';

    $inctype_id = GET('id');
    
    ossim_valid($inctype_id, OSS_ALPHA, OSS_SPACE, OSS_PUNC, 'illegal:'._("Incident type"));

    if (ossim_error()) {
           die(ossim_error());
    }

    require_once ('ossim_db.inc');
    require_once ("classes/Incident_type.inc");
    
    $db = new ossim_db();
    $conn = $db->connect();

    if ($inctype_list = Incident_type::get_list($conn, "WHERE id = '$inctype_id'")) {
                  $inctype = $inctype_list[0];
    }
?>

<form method="post" action="modifyincidenttype.php">
<table align="center">
  <input type="hidden" name="modify" value="modify" />
  <input type="hidden" name="id" value="<?php echo $inctype->get_id(); ?>" />
  <tr>
    <th> <?php echo gettext("Incident type"); ?> </th>
    <th class="left"><?php echo $inctype->get_id(); ?></th>
  </tr>
  <tr>
    <th> <?php echo gettext("Description"); ?> </th>
    <td class="left">
      <textarea name="descr"><?php echo $inctype->get_descr(); ?></textarea>
    </td>
  </tr>
  <tr>
    <td colspan="2" align="center">
      <input type="submit" value="OK">
      <input type="reset" value="reset">
    </td>
  </tr>
</table>
</form>

</body>
</html>

