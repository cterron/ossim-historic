<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuTools", "ToolsUserLog");
?>

<html>
<head>
  <title> User action logs </title>
  <meta http-equiv="refresh" content="150">
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>

<body>
  
  <h1> <?php echo gettext("User Action logs"); ?> </h1>
  
<?php
require_once 'ossim_db.inc';
require_once 'classes/Util.inc';
require_once 'classes/Alarm.inc';
require_once 'classes/Log_action.inc';
require_once 'classes/Log_config.inc';
require_once 'classes/Security.inc';

/* number of logs per page */
$ROWS = 50;

$order = GET('order');
$inf = GET('inf');
$sup = GET('sup');
$user = GET('user');
$code = GET('code');

ossim_valid($order, OSS_ALPHA, OSS_SPACE, OSS_SCORE, OSS_NULLABLE, 'illegal:'._("order"));
ossim_valid($inf, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("inf"));
ossim_valid($sup, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("order"));
ossim_valid($user, OSS_USER, OSS_NULLABLE, 'illegal:'._("hide_closed"));
ossim_valid($code, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("hide_closed"));

if (ossim_error()) {
    die(ossim_error());
}

/* connect to db */
$db = new ossim_db();
$conn = $db->connect();

if (empty($order)) $order = "date";

if (empty($inf)) $inf = 0;
if (empty($sup)) $sup = $ROWS;

?>

    <!-- filter -->
    <form name="logfilter" method="GET" action="<?php echo $_SERVER["PHP_SELF"]
?>">
    <table align="center">
      <tr colspan="3">
        <th colspan="2"> <?php echo gettext("Filter"); ?> </th>
      </tr>
      <tr>
      <td>
        <?php echo gettext("User"); ?>
      </td>
      <td>
         <?php echo gettext("Action"); ?>
      </td>
      </tr>
      <tr>
      <td>
        <select name="user" onChange="document.forms['logfilter'].submit()">
        <?php
        require_once('classes/Session.inc');
         ?>
                <option <?php if ("" == $user)  echo " selected "?>
                 value="">All</option>"; ?>
        <?php
        if ($session_list = Session::get_list($conn, "ORDER BY login")) {
                 foreach ($session_list as $session) {
                $login = $session->get_login();
        ?>
                 <option  <?php if ($login == $user) echo " selected "; ?>
                  value="<?php echo $login; ?>"><?php echo $login; ?>
                </option>                
        <?php         
                 }
                 }
        ?>
        </select>
      </td>
      <td>
        <select name="code" onChange="document.forms['logfilter'].submit()">
        <?php
        require_once('classes/Session.inc');
         ?>
                <option <?php if ("" == $code)  echo " selected "?>
                 value="">All</option>"; ?>
        <?php
        if ($code_list = Log_config::get_list($conn, "ORDER BY code")) {
                 foreach ($code_list as $code_log) {
                $code_aux = $code_log->get_code();
        ?>
                 <option  <?php if ($code_aux == $code) echo " selected "; ?>
                  value="<?php echo $code_aux; ?>"><?php echo
                  "[".$code."]".preg_replace('|%.*?%|'," ",$code_log->get_descr( )); ?>
                </option>                
        <?php         
                 }
                 }
        ?>
        </select>
      </td>
      </tr>  

    </table><br>
    
    <table width="100%">
      <tr>
        <td colspan="6">
<?php

    $cfilter = ""; 
    $filter = "";
    if (!empty($user)) {
        $filter = " and '$user' = log_action.login ";
   } 
   
    if (!empty($code)) {
        $filter .= " and '$code' = log_action.code";
    }

    if ((!empty($code)) and (!empty($user))){
        $cfilter = "where '".$user."' = log_action.login and
        '".$code."' = code";
    } else {
       if (!empty($code)){ $cfilter = "where
           '".$code."' = code"; }
       if (!empty($user)){ $cfilter = "where
           '".$user."' = login"; }
    }
    
    
    /* 
     * prev and next buttons 
     */
    $inf_link = $_SERVER["PHP_SELF"] . 
            "?order=$order" . 
            "&sup=" . ($sup - $ROWS) .
            "&inf=" . ($inf - $ROWS);
    $sup_link = $_SERVER["PHP_SELF"] . 
        "?order=$order" . 
        "&sup=" . ($sup + $ROWS) .
        "&inf=" . ($inf + $ROWS);

    
    $count = Log_action::get_count($conn,$cfilter);
    
    if ($inf >= $ROWS) {
        echo "<a href=\"$inf_link\">&lt;-"; printf(gettext("Prev %d"),$ROWS); echo "</a>";
    }
    if ($sup < $count) {
        echo "&nbsp;&nbsp;("; printf(gettext("%d-%d of %d"),$inf, $sup, $count); echo ")&nbsp;&nbsp;";
        echo "<a href=\"$sup_link\">"; printf(gettext("Next %d"), $ROWS); echo " -&gt;</a>";
    } else {
        echo "&nbsp;&nbsp;("; printf(gettext("%d-%d of %d"),$inf, $count, $count); echo ")&nbsp;&nbsp;";
    }
?>
        </td>
      </tr>
    
      <tr>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php echo
        ossim_db::get_order("date", $order);?>">
        <?php echo
        gettext("Date"); ?></a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php echo
        ossim_db::get_order("login", $order);?>">
        <?php echo
        gettext("User"); ?></a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php echo
        ossim_db::get_order("ipfrom", $order);?>">
        <?php echo
        gettext("ip"); ?></a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php echo
        ossim_db::get_order("code", $order);?>">
        <?php echo
        gettext("Code"); ?></a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php echo
        ossim_db::get_order("info", $order);?>">
        <?php echo
        gettext("Action"); ?></a></th>
      </tr>

<?php
    $time_start = time();

   
     
    if ($log_list = Log_action::get_list($conn, $filter,"ORDER by $order", $inf, $sup))
    {
        foreach ($log_list as $log) {
?>
        <tr>
        <td><?php echo $log->get_date(); ?>         
        </td>
        
        <td><?php echo $log->get_login(); ?>         
        </td>
        
        <td><?php echo $log->get_from(); ?>         
        </td>
        
        <td><?php echo $log->get_code(); ?>         
        </td>
        
        <td><?php echo $log->get_info(); ?>         
        </td>
        
      </td>
      </tr>
<?php

} /* foreach alarm_list */
?>
      <tr>
        <td></td>
        <td colspan="8">
        </td>
      </tr>
<?php
    } /* if alarm_list */
?>
    </table>


<?php
$time_load = time() - $time_start;
echo "[ ".gettext("Page loaded in")." $time_load ".gettext("seconds")." ]";
$db->close($conn);
?>

</body>
</html>
