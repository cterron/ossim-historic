<?php
header('Content-Type: text/xml');

    require_once 'ossim_db.inc';
    require_once 'classes/Incident.inc';
    require_once 'classes/Security.inc';
    
    $q = GET('q');

    ossim_valid($q, OSS_NULLABLE, OSS_SPACE, OSS_SCORE, OSS_ALPHA, OSS_PUNC, OSS_AT, 'illegal:'._("q"));

    if (ossim_error()) {
        die(ossim_error());
    }
                        
      $db = new ossim_db();
      $conn = $db->connect();


      $countquery = OssimQuery("SELECT count(*) as count from incident_ticket
      where description like \"%$q%\"");
      $query = OssimQuery("SELECT description from incident_ticket where
      description like \"%$q%\"");

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

