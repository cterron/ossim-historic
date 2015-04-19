<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicySignatures");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>
<body>

  <h1> <?php echo gettext("Signatures"); ?> </h1>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Signature_group.inc';
    require_once 'classes/Security.inc';

    $order = GET('order');

    ossim_valid($order, OSS_ALPHA, OSS_SPACE, OSS_SCORE, OSS_NULLABLE, 'illegal:'._("order"));
    
    if (ossim_error()) {
        die(ossim_error());
    }
   
    if (empty($order)) $order = "name";
?>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("name", $order); ?>"> 
	    <?php echo gettext("Name"); ?> </a></th>
      <th> <?php echo gettext("Signatures"); ?> </th>
      <th> <?php echo gettext("Description"); ?> </th>
      <th> <?php echo gettext("Action"); ?> </th>
    </tr>
<?php
    
    $db = new ossim_db();
    $conn = $db->connect();
    
    if ($signature_list = Signature_group::get_list($conn)) {
        foreach (Signature_group::get_list($conn, "ORDER BY $order") 
                 as $sig_group) {
            $sig_group_name = $sig_group->get_name();
?>
    <tr>
      <td><?php echo $sig_group_name; ?></td>
      <td>
<?php
    foreach ($sig_group->get_reference_signatures($conn, $sig_group_name) 
             as $sig) {
        echo $sig->get_sig_name() . "<br>";
    }
?>
      </td>
      <td><?php echo $sig_group->get_descr(); ?></td>
      <td>
        <a href="modifysignatureform.php?signame=<?php 
            echo $sig_group->get_name()?>"> 
	    <?php echo gettext("Modify"); ?> </a>
        <a href="deletesignature.php?signame=<?php
            echo $sig_group->get_name()?>"> 
	    <?php echo gettext("Delete"); ?> </a></td>
    </tr>
<?php
        }
    }
?>
    <tr>
      <td colspan="4" align="center">
        <a href="newsignatureform.php"> <?php echo gettext("Insert new Signature Group"); ?> </a>
      </td>
    <tr>
      <td colspan="4"><a href="../conf/reload.php?what=signatures"> <?php echo gettext("Reload"); ?> </a></td>
    </tr>
    </td>
</table>

