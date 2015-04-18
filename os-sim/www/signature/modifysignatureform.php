<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Modify signature group</h1>

<?php
    require_once 'classes/Signature_group.inc';
    require_once 'classes/Signature.inc';
    require_once 'classes/Signature_group_reference.inc';
    require_once 'ossim_db.inc';
    $db = new ossim_db();
    $conn = $db->connect();

    if (!$sig_name = mysql_escape_string($_GET["signame"])) {
        echo "<p>Wrong signature name</p>";
        exit;
    }

    if ($signature_group_list = Signature_group::get_list
            ($conn, "WHERE name = '$sig_name'")) {
        $sig_group = $signature_group_list[0];
    }
?>
<form method="post" action="modifysignature.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th>Name</th>
        <input type="hidden" name="name"
               value="<?php echo $sig_group->get_name(); ?>">
    <td class="left">
      <b><?php echo $sig_group->get_name(); ?></b>
    </td>
  </tr>
  <tr>
    <th>Signatures</th>
    <td class="left">
<?php

    $i = 1;
    if ($signature_list = Signature::get_list($conn)) {
        foreach ($signature_list as $sig) {
            if ($i == 1) {
?>
        <input type="hidden" name="nsigs"
            value="<?php echo count(Signature::get_list($conn)); ?>">
<?php
            } $name = "mbox" . $i;
?>
        <input type="checkbox" 
<?php
            if (Signature_group_reference::in_signature_group_reference
                    ($conn, $sig_group->get_name(), $sig->get_name()))
            {
                echo " CHECKED ";
            }
?>
            name="<?php echo $name;?>"
            value="<?php echo $sig->get_name(); ?>">
            <?php echo $sig->get_name()."<br>"; ?>
        </input>
<?php
            $i++;
        }
    }
    $db->close($conn);
?>
    </td>
  </tr>
  <tr>
    <th>Description</th>
    <td class="left">
      <textarea name="descr" rows="2" 
        cols="20"><?php echo $sig_group->get_descr();?></textarea>
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

