<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicySignatures");
?>

<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>
<body>

  <h1>Signatures</h1>

<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Signature_group.inc';
    
    if (!$order = $_GET["order"]) $order = "name";
?>

  <table align="center">
    <tr>
      <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("name", $order);
          ?>">Name</a></th>
      <th>Signatures</th>
      <th>Description</th>
      <th>Action</th>
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
            echo $sig_group->get_name()?>">Modify</a>
        <a href="deletesignature.php?signame=<?php
            echo $sig_group->get_name()?>">Delete</a></td>
    </tr>
<?php
        }
    }
?>
    <tr>
      <td colspan="4" align="center">
        <a href="newsignatureform.php">Insert new Signature Group</a>
      </td>
    <tr>
      <td colspan="4"><a href="../conf/reload.php?what=signatures">Reload</a></td>
    </tr>
    </td>
</table>

