<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicyServers");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Delete server"); ?> </h1>

<?php 

require_once 'classes/Security.inc';

$name = GET('name');

ossim_valid($name, OSS_ALPHA, OSS_PUNC, OSS_SPACE, OSS_SCORE, 'illegal:'._("Server name"));

if (ossim_error()) {
    die(ossim_error());
}
                    

if (GET('confirm')) {
?>
    <p> <?php echo gettext("Are you sure?"); ?> </p>
    <p><a 
      href="<?php echo $_SERVER["PHP_SELF"]."?name=$name&confirm=yes"; ?>">
      <?php echo gettext("Yes"); ?> </a>
      &nbsp;&nbsp;&nbsp;<a href="server.php"> 
      <?php echo gettext("No"); ?> </a>
    </p>
<?php
    exit();
}

    require_once 'ossim_db.inc';
    require_once 'classes/Server.inc';
    $db = new ossim_db();
    $conn = $db->connect();
    Server::delete($conn, $name);
    $db->close($conn);

?>

    <p> <?php echo gettext("Server deleted"); ?> </p>
    <p><a href="server.php"> 
    <?php echo gettext("Back"); ?> </a></p>
<?
// update indicators on top frame
$OssimWebIndicator->update_display();
?>

</body>
</html>

