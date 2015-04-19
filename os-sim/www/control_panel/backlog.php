<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuCorrelation", "CorrelationBacklog");
?>

<html>
<head>
  <title> <?php echo gettext("Control Panel"); ?> </title>
  <meta http-equiv="refresh" content="150">
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>

<body>

  <h1 align="center"> <?php echo gettext("Backlog"); ?> </h1>

<?php
require_once ('ossim_db.inc');
require_once ('common.inc');
require_once ('classes/Host.inc');
require_once ('classes/Backlog.inc');
require_once ('classes/Plugin_sid.inc');


/* connect to db */
$db = new ossim_db();
$conn = $db->connect();

if ($id = $_GET["delete"]) {
    Backlog::delete($conn, $id);
}

if (!$order = $_GET["order"]) $order = "id";

if (($src_ip = $_GET["src_ip"]) && ($dst_ip = $_GET["dst_ip"])) {
    $where = "WHERE inet_ntoa(src_ip) = '$src_ip' OR inet_ntoa(dst_ip) = '$dst_ip'";
} elseif ($src_ip = $_GET["src_ip"]) {
    $where = "WHERE inet_ntoa(src_ip) = '$src_ip'";
} elseif ($dst_ip = $_GET["dst_ip"]) {
    $where = "WHERE inet_ntoa(dst_ip) = '$dst_ip'";
} else {
    $where = '';
}

if (!$inf = $_GET["inf"])
    $inf = 0;
if (!$sup = $_GET["sup"])
    $sup = 25;

?>
    <table width="100%">
      <tr>
        <td colspan="5">
<?php

    /* 
     * prev and next buttons 
     */
    $inf_link = $_SERVER["PHP_SELF"] . 
            "?order=$order" . 
            "&sup=" . ($sup - 25) .
            "&inf=" . ($inf - 25);
    $sup_link = $_SERVER["PHP_SELF"] . 
        "?order=$order" . 
        "&sup=" . ($sup + 25) .
        "&inf=" . ($inf + 25);
    $count = Backlog::get_count($conn);
    
    if ($inf >= 25) {
        echo "<a href=\"$inf_link\">&lt;- "; printf(gettext("Prev %d"), 25); echo "</a>";
    }
    echo "&nbsp;&nbsp;("; printf(gettext("%d-%d of %d"),$inf, $sup, $count); echo ")&nbsp;&nbsp;";
    if ($sup < $count) {
        echo "<a href=\"$sup_link\"> "; printf(gettext("Next %d"), 25); echo " -&gt;</a>";
    }
?>
        </td>
      </tr>
      
      <tr>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("id", $order) .
            "&inf=$inf&sup=$sup"
            ?>"> <?php echo gettext("Id"); ?> </a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("timestamp", $order) .
            "&inf=$inf&sup=$sup"
            ?>"> <?php echo gettext("Date"); ?> </a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("directive_id", $order) .
            "&inf=$inf&sup=$sup"
            ?>"> <?php echo gettext("Directive"); ?> </a></th>
        <th><a href="<?php echo $_SERVER["PHP_SELF"]?>?order=<?php
            echo ossim_db::get_order("matched", $order) . 
            "&inf=$inf&sup=$sup"
            ?>"> <?php echo gettext("Matched"); ?> </a></th>
        <th> <?php echo gettext("Delete"); ?> </th>
      </tr>
<?php
    if ($backlog_list = Backlog::get_list($conn, 
                                          "$where ORDER BY $order",
                                          $inf, $sup))
    {
        foreach($backlog_list as $backlog) {

            $sid = $backlog->get_directive_id();
            $sid_name = "";
            if ($plugin_sid_list = Plugin_sid::get_list
                ($conn, "WHERE plugin_id = 1505 AND sid = $sid")) {
                $sid_name = $plugin_sid_list[0]->get_name();
            } else {
                $sid_name = "Unknown directive";
            }
 

?>
      <tr>
      <td bgcolor="#eeeeee"><?php echo $backlog->get_id(); ?></td>
      <td nowrap><?php echo timestamp2date ($backlog->get_timestamp()) ?></td>
      <td><?php echo ereg_replace("directive_alert: ", "", $sid_name) . 
                " (" . $backlog->get_directive_id() . ") "; ?></td>
      <td><?php 
        if ($backlog->get_matched() == 0) {
            echo gettext("NO");
        } else {
            echo "<b> " . gettext("YES") . " </b>";
        }
      ?></td>
      <td><a href="<?php echo $_SERVER["PHP_SELF"] ?>?delete=<?php 
            echo $backlog->get_id() ?>"> <?php echo gettext("Delete"); ?> </a></td>
      </tr>
<?php
        } /* foreach backlog_list */
?>
      <tr>
        <td colspan="5"><a href="<?php 
            echo $_SERVER["PHP_SELF"] ?>?delete=all"> <?php echo gettext("Delete ALL"); ?> </a>
        </td>
      </tr>
<?php
    } /* if backlog_list */
?>
    </table>


<?php
$db->close($conn);
?>

</body>
</html>


