<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>
<body>

  <h1>OSSIM Framework</h1>
  <h2>Signatures</h2>

<table align="center">
    <tr>
      <th>Name</th><th>Signatures</th><th>Description</th>
      <th>Action</th>
    </tr>
<?php
    require_once 'ossim_db.inc';
    require_once 'classes/Signature_group.inc';
    
    $db = new ossim_db();
    $conn = $db->connect();
    
    if ($signature_list = Signature_group::get_list($conn)) {
        foreach (Signature_group::get_list($conn) as $sig_group) {
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
    </td>
</table>

