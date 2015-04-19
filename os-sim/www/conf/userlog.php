<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationUserlog");
?>

<html>
<head>
  <title> <?php echo gettext("User logging Configuration"); ?> </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>

<body>


<?php

	require_once ('ossim_db.inc');
	require_once ('classes/Log_config.inc');
	require_once ('classes/Security.inc');
    
	/* connect to db */
	$db = new ossim_db();
	$conn = $db->connect();
?>

<form method="POST" action="<?php echo $_SERVER["PHP_SELF"] ?>" />
 <table align=center>


<?php
function submit ($conn)
{
?>
    <tr>
      <td colspan="3">
        <input type="submit" name="update"
            value=" <?php echo gettext("Update configuration"); ?> " />
      </td>
    </tr>
<?php

if (POST('update'))
{
                  
    for ($i = 1; $i <= POST('nconfs'); $i++) 
    {
        if (POST("value_$i") == 'on'){
            Log_config::update_log($conn,$i,'1');
        } else {
            Log_config::update_log($conn,$i,'0');
        }
    }
}

}// submit

?>

<?php
	submit($conn);
?>
<tr>
    <th>#</th>
    <th><?php echo gettext("Action description"); ?></th>
    <th>#</th>
</tr>

<?php
    $max = 0;
    if ($log_conf_list = Log_config::get_list($conn, "ORDER BY descr"))
    {

	   foreach($log_conf_list as $log_conf) 
	   {

?>
	        
       <tr>
	   <td><?php echo $log_conf->get_code(); ?></td>
	   <td><?php echo preg_replace('|%.*?%|'," ",$log_conf->get_descr( )); ?></td>

            <?php $input = "<input type=CHECKBOX
            name=\"value_".$log_conf->get_code()."\"";
       	if ($log_conf->get_log()) {
		    $input .= "CHECKED >";
		} else {
		    $input .= ">";
		}		
	    ?>

	   <td><?php echo $input; ?></td>
	   </tr>
<?php
       $input = "";
	   $max = max($max, $log_conf->get_code());
	  
	   }
	
	}

?>

<?php submit($conn); ?>    
 </table> 
<input type="hidden" name="nconfs" value="<?php echo $max ?>" />
</form>
         
<?php
$db->close($conn);
?>
</body>
</html>

