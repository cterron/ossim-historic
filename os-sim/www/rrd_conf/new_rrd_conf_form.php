<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>OSSIM Framework</h1>
  <h2>New RRD Config</h2>
<?php
require_once 'classes/RRD_conf.inc';
require_once 'classes/RRD_data.inc';
?>
<?php $DEFAULT_THRESHOLD = 100 ?>
<?php $DEFAULT_PRIORITY = 5 ?>
<?php $DEFAULT_ALPHA = 0.1 ?>
<?php $DEFAULT_BETA = 0.0035 ?>

<form method="post" action="new_rrd_conf.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th>IP</th>
    <th class="center">
        <input type="text" name="ip">
    </th>
  </tr>
    <tr>
    <th>Modify</th><th> Threshold / Priority / Alpha / Beta</th>
    </tr>
    <?php
    $count_values = count($rrd_values);
    $count_names = count($rrd_names);
    if($count_values != $count_names){
    print "Consistency check failed, please check RRD_data.inc\n";
    exit;
    }
    foreach($rrd_names as $key => $value) {
    ?>
    <tr>
    <th><?php print $value?></th>
    <td class="center">

      <input type="text" name="<?php echo $key?>_threshold" size="5" 
             value="<?php echo $DEFAULT_THRESHOLD ?>">
      <input type="text" name="<?php echo $key?>_priority" size="5" 
             value="<?php echo $DEFAULT_PRIORITY ?>">
      <input type="text" name="<?php echo $key?>_alpha" size="5" 
             value="<?php echo $DEFAULT_ALPHA ?>">
      <input type="text" name="<?php echo $key?>_beta" size="5" 
             value="<?php echo $DEFAULT_BETA ?>">
    </td>
  </tr>
<?php
    }
    ?>
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

