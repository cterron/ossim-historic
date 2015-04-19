<?php
header('Content-Type: text/xml');

    require_once 'ossim_db.inc';
    require_once 'classes/Incident.inc';

      $db = new ossim_db();
      $conn = $db->connect();

      $q = mysql_real_escape_string($_GET['q']);

      $countquery = "SELECT count(*) as count from incident_ticket where description like \"%$q%\";";
      $query = "SELECT description from incident_ticket where description like \"%$q%\";";

        if (!$rs = &$conn->Execute($countquery)) {
            print $conn->ErrorMsg();
        } else {
            $num = $rs->fields["count"];
            if($num == 0){
            ?>
<response>
<method>0</method>
<result>no results</result>
</response>
<?php
            exit;
            }
            if (!$rs = &$conn->Execute($query)) {
                print $conn->ErrorMsg();
            } else {
                ?>
<response>
                <?php
                while (!$rs->EOF) {
                    ?>
<method><?php echo $num; ?></method>
<result><?php echo $rs->fields["description"];?></result>
                    <?php
                    $rs->MoveNext();
                }
                ?>
</response>
            <?php
                exit;
            }
      }
      // Shouldn't be reached
?>
<response>
  <method>0</method>
  <result>no results</result>
</response>

