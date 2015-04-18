<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuConfiguration", "ConfigurationHostScan");
?>

<?php
    if ($_POST["delete"]) {

        require_once('classes/Scan.inc');
        require_once 'ossim_db.inc';
                                                                                
        $db = new ossim_db();
        $conn = $db->connect();

        Scan::delete_all($conn);

        $db->close($conn);
?>
<?php
    }
?>

    <form method="post" action="<?php echo $_SERVER["PHP_SELF"]?>">
      <input type="hidden" name="delete" value="delete">
      <input type="submit" value="Delete">
    </form>

